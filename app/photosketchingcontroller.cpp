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


void PhotoSketchingController::newSketch()
{
  mCurrentLine = ColorPath( mPenColor, {} );
  // drawing with stylus adds these shadow sketches with just one point, which break the undo user experience
  if ( !mActivePaths.isEmpty() && mActivePaths.last().mPoints.size() == 1 )
  {
    mPaths.removeLast();
    mActivePaths.removeLast();
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
  const QString photoFileName = QUrl( mPhotoSource ).fileName();
  const QString photoPath = QString( "%1/%2/%3" ).arg( QDir::tempPath(), mProjectName, photoFileName );
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

  const int pathCount = static_cast<int>( mActivePaths.size() );

  for ( int i = 0; i < pathCount; ++i )
  {
    const QColor color = mActivePaths.at( i ).mColor;
    const QVector<QPointF> points = mActivePaths.at( i ).mPoints;

    if ( points.isEmpty() )
      continue;

    const QPen pen( color, SKETCH_WIDTH * mPhotoScale );
    painter.setPen( pen );

    QPainterPath path;
    path.moveTo( points.first() );
    for ( int j = 1; j < points.size(); ++j )
    {
      path.lineTo( points.at( j ) );
    }

    painter.drawPath( path );
  }

  if ( !image.save( photoPath ) )
  {
    CoreUtils::log( "Photo sketching", "Failed to save temporary image to: " + photoPath );
  }
  else
  {
    if ( ImageUtils::copyExifMetadata( QUrl( mPhotoSource ).toLocalFile(), photoPath ) && ImageUtils::clearOrientationMetadata( photoPath ) )
    {
      CoreUtils::log( "Photo sketching", "Temporary image saved to: " + photoPath );
      emit tempPhotoSourceChanged( photoPath );
    }
    else
    {
      CoreUtils::log( "Photo sketching", "Failed to copy metadata to: " + photoPath );
    }
  }

  mActivePaths.clear();
  mCanUndo = false;
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
  colorPath.mPoints = shapePoints;

  return colorPath;
}

QUrl PhotoSketchingController::getCurrentPhotoPath() const
{
  const QString photoFileName = QUrl( mPhotoSource ).fileName();
  const QString photoPath = QString( "%1/%2/%3" ).arg( QDir::tempPath(), mProjectName, photoFileName );
  if ( QFile::exists( photoPath ) )
  {
    return { photoPath };
  }

  return { mOriginalPhotoSource };
}

void PhotoSketchingController::prepareController()
{
  mProjectName = QUrl::fromLocalFile( mProjectName ).fileName();
  mOriginalPhotoSource = QUrl( mPhotoSource ).toLocalFile();
  const QString photoFileName = QUrl( mPhotoSource ).fileName();
  const QString savePath = QString( "%1/%2/%3" ).arg( QDir::tempPath(), mProjectName, photoFileName );
  if ( !photoFileName.isEmpty() && QFile::exists( savePath ) )
  {
    mPhotoSource = QUrl::fromLocalFile( savePath ).toString();
    emit tempPhotoSourceChanged( savePath );
  }
}

int PhotoSketchingController::sketchWidth()
{
  return SKETCH_WIDTH;
}
