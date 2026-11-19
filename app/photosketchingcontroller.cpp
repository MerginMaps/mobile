/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "photosketchingcontroller.h"

#include <QColor>
#include <QImageReader>
#include <QPointF>
#include <QUrl>

#include "coreutils.h"
#include "imageutils.h"
#include "inpututils.h"

/**
 * ColorPath declarations
 */


ColorPath::ColorPath()
{
  mColor = QColor();
  mPoints = QVector<QPointF>();
}

bool ColorPath::operator==( const ColorPath &other ) const
{
  if ( mColor == other.mColor  &&  mPoints == other.mPoints )
  {
    return true;
  }

  return false;
}


/**
 * PhotoDrawingController declarations
 */


namespace
{
  // a single point is not drawn, so a dot is drawn as a tiny line
  QVector<QPointF> drawablePoints( const QVector<QPointF> &points, bool keepDots )
  {
    if ( keepDots && points.size() == 1 )
    {
      return { points.first(), points.first() + QPointF( 0.1, 0 ) };
    }
    return points;
  }
}

void PhotoSketchingController::newSketch()
{
  mCurrentLine = ColorPath( mPenColor, {} );
  // drawing with stylus adds these shadow sketches with just one point, which break the undo user experience
  if ( !mSignatureMode && !mActivePaths.isEmpty() && mActivePaths.last().mPoints.size() == 1 )
  {
    mPaths.removeLast();
    mActivePaths.removeLast();
    // QML syncs its shapes with mPaths through this signal
    emit lastPathRemoved();

    if ( mActivePaths.empty() && mCanUndo )
    {
      mCanUndo = false;
      emit canUndoChanged();
    }
  }
}

void PhotoSketchingController::addPoint( const QPointF &newPoint )
{
// we scale up the point to picture's true position
  mCurrentLine.mPoints.append( newPoint * mPhotoScale );
  if ( mActivePaths.isEmpty() || mCurrentLine.mPoints.size() == 1 )
  {
    mPaths.append( mCurrentLine );
    mActivePaths.append( mCurrentLine );
    emit newPathAdded( -1 );
    if ( !mCanUndo && !mPaths.isEmpty() )
    {
      mCanUndo = true;
      emit canUndoChanged();
    }
  }
  else
  {
    mPaths.last() = mCurrentLine;
    mActivePaths.last() = mCurrentLine;
    emit pathUpdated( {static_cast<int>( mPaths.size() - 1 ) } );
  }
}

void PhotoSketchingController::setActiveColor( const QColor &newColor )
{
  mPenColor = newColor;
  mCurrentLine.mColor = newColor;

  emit activeColorChanged();
}

void PhotoSketchingController::undo()
{
  if ( !mActivePaths.empty() )
  {
    mPaths.removeLast();
    mActivePaths.removeLast();
    emit lastPathRemoved();
  }

  // the next point starts a new line
  mCurrentLine = ColorPath( mPenColor, {} );

  if ( !mActivePaths.empty() && !mCanUndo )
  {
    mCanUndo = true;
    emit canUndoChanged();
  }
  else if ( mActivePaths.empty() && mCanUndo )
  {
    mCanUndo = false;
    emit canUndoChanged();
  }
}

void PhotoSketchingController::clear()
{
  mPenColor = QColor( Qt::white );
  emit activeColorChanged();
  newSketch();
  mCanUndo = false;
  emit canUndoChanged();
  if ( !mPaths.isEmpty() ) mPaths.clear();
  if ( !mActivePaths.isEmpty() ) mActivePaths.clear();
  emit pathsReset();
}

void PhotoSketchingController::backupSketches()
{
  if ( mSignatureMode )
  {
    backupSignature();
    mActivePaths.clear();
    mCanUndo = false;
    return;
  }

  const QString photoPath = tempSketchPath( mProjectName, QUrl( mPhotoSource ).fileName() );
  if ( !QFile::exists( photoPath ) )
  {
    // create new temp file
    InputUtils::copyFile( mPhotoSource, photoPath );
  }

  QImageReader imageReader( photoPath );
  imageReader.setAutoTransform( true );
  QImage image = imageReader.read();
  if ( image.isNull() )
  {
    CoreUtils::log( "Photo sketching", "Failed to load image from: " + photoPath );
    CoreUtils::log( "Photo sketching", "Error: " + imageReader.errorString() );
    return;
  }

  QPainter painter( &image );
  painter.setRenderHint( QPainter::Antialiasing );
  drawPaths( painter );

  if ( !image.save( photoPath ) )
  {
    CoreUtils::log( "Photo sketching", "Failed to save temporary image to: " + photoPath );
  }
  else
  {
    if ( ImageUtils::copyExifMetadata( mOriginalPhotoSource, photoPath ) && ImageUtils::clearOrientationMetadata( photoPath ) )
    {
      CoreUtils::log( "Photo sketching", "Temporary image saved to: " + photoPath );
      emit tempPhotoSourceChanged( photoPath );
    }
    else
    {
      CoreUtils::log( "Photo sketching", "Failed to copy metadata to: " + photoPath );
      InputUtils::removeFile( photoPath );
      clear();
      emit sketchesSavingError();
    }
  }

  mActivePaths.clear();
  mCanUndo = false;
}

void PhotoSketchingController::backupSignature()
{
  const QString signaturePath = tempSketchPath( mProjectName, QUrl( mPhotoSource ).fileName() );
  if ( mActivePaths.isEmpty() )
  {
    CoreUtils::log( "Sign feature", "Nothing drawn, signature not saved to: " + signaturePath );
    emit sketchesSavingError();
    return;
  }

  // the image is only as large as the signature plus padding
  const QRectF bounds = sketchBounds();
  const qreal padding = std::max( bounds.width(), bounds.height() ) / 25 + SKETCH_WIDTH * mPhotoScale;
  const QRect imageRect = bounds.adjusted( -padding, -padding, padding, padding ).toAlignedRect();

  QImage image( imageRect.size(), QImage::Format_RGB32 );
  image.fill( Qt::white );

  QPainter painter( &image );
  painter.setRenderHint( QPainter::Antialiasing );
  painter.translate( -imageRect.topLeft() );
  drawPaths( painter );
  painter.end();

  if ( !InputUtils::createDirectory( QFileInfo( signaturePath ).absolutePath() ) || !image.save( signaturePath, "PNG" ) )
  {
    CoreUtils::log( "Sign feature", "Failed to save signature to: " + signaturePath );
    emit sketchesSavingError();
    return;
  }

  CoreUtils::log( "Sign feature", "Signature saved to: " + signaturePath );
  emit tempPhotoSourceChanged( signaturePath );
}

void PhotoSketchingController::drawPaths( QPainter &painter ) const
{
  for ( const ColorPath &colorPath : mActivePaths )
  {
    const QVector<QPointF> points = drawablePoints( colorPath.mPoints, mSignatureMode );
    if ( points.isEmpty() )
      continue;

    painter.setPen( QPen( colorPath.mColor, SKETCH_WIDTH * mPhotoScale ) );

    QPainterPath path;
    path.moveTo( points.first() );
    for ( int i = 1; i < points.size(); ++i )
    {
      path.lineTo( points.at( i ) );
    }

    painter.drawPath( path );
  }
}

void PhotoSketchingController::removeBackupSketches()
{
  const QString photoPath = tempSketchPath( mProjectName, QUrl( mPhotoSource ).fileName() );
  if ( QFile::exists( photoPath ) )
  {
    InputUtils::removeFile( photoPath );
    CoreUtils::log( "Photo sketching", "Removed temporary image from: " + photoPath );
  }
}

void PhotoSketchingController::redrawPaths()
{
  for ( int i = 0 ; i < mPaths.size(); ++i )
  {
    emit newPathAdded( i );
  }
}

void PhotoSketchingController::setPhotoScale( const double newRatio )
{
  mPhotoScale = newRatio;
// we want to update all paths
  if ( !mPaths.isEmpty() )
  {
    QVector<int> changedPathIndexes;
    for ( int i = 0; i < mPaths.size(); i++ )
    {
      changedPathIndexes.append( i );
    }
    emit pathUpdated( changedPathIndexes );
  }
}

ColorPath PhotoSketchingController::getPath( const int row ) const
{
  if ( row < -mPaths.size() || row >= mPaths.size() ) return {};

  int index = row;
  if ( row < 0 )
  {
    index = static_cast<int>( mPaths.size() ) + row;
  }

  ColorPath colorPath( mPaths.at( index ) );

// we recalculate stored points into the coordinates of printed picture
  QVector<QPointF> shapePoints;
  for ( const QPointF &point : colorPath.mPoints )
  {
    shapePoints.append( point / mPhotoScale );
  }
  colorPath.mPoints = drawablePoints( shapePoints, mSignatureMode );

  return colorPath;
}

QUrl PhotoSketchingController::getCurrentPhotoPath() const
{
  const QString photoPath = tempSketchPath( mProjectName, QUrl( mPhotoSource ).fileName() );
  if ( QFile::exists( photoPath ) )
  {
    return { photoPath };
  }

  return { mOriginalPhotoSource };
}

QRectF PhotoSketchingController::sketchBounds() const
{
  if ( mPaths.isEmpty() )
    return {};

  // QRectF::united() ignores the zero sized rectangle of a single point, so the bounds are collected by hand
  QPointF topLeft = mPaths.first().mPoints.first();
  QPointF bottomRight = topLeft;
  for ( const ColorPath &path : mPaths )
  {
    for ( const QPointF &point : path.mPoints )
    {
      topLeft = QPointF( std::min( topLeft.x(), point.x() ), std::min( topLeft.y(), point.y() ) );
      bottomRight = QPointF( std::max( bottomRight.x(), point.x() ), std::max( bottomRight.y(), point.y() ) );
    }
  }

  return { topLeft, bottomRight };
}

void PhotoSketchingController::prepareController()
{
  mProjectName = QUrl::fromLocalFile( mProjectName ).fileName();
  mOriginalPhotoSource = QUrl( mPhotoSource ).toLocalFile();
  const QString photoFileName = QUrl( mPhotoSource ).fileName();
  const QString savePath = tempSketchPath( mProjectName, photoFileName );
  if ( !photoFileName.isEmpty() && QFile::exists( savePath ) )
  {
    mPhotoSource = QUrl::fromLocalFile( savePath ).toString();
    emit tempPhotoSourceChanged( savePath );
  }
}

QString PhotoSketchingController::tempSketchPath( const QString &projectHomePath, const QString &fileName )
{
  const QString projectName = QUrl::fromLocalFile( projectHomePath ).fileName();
  return QString( "%1/%2/%3" ).arg( QDir::tempPath(), projectName, fileName );
}

int PhotoSketchingController::sketchWidth()
{
  return SKETCH_WIDTH;
}
