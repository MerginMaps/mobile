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
#include <QPointF>
#include <QPainterPath>
#include <QPainter>
#include <QUrl>

#include "coreutils.h"

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


void PhotoSketchingController::newDrawing()
{
  mCurrentLine = ColorPath( mPenColor, {} );
  mPaths.append( mCurrentLine );
  emit newPathAdded();
  if ( !mCanUndo && mPaths.size() > 1 )
  {
    mCanUndo = true;
    emit canUndoChanged();
  }
}

void PhotoSketchingController::addPoint( const QPointF newPoint )
{
// we scale up the point to picture's true position
  const QPointF realPoint = QPointF( newPoint.x() * mPhotoScaleRatio, newPoint.y() * mPhotoScaleRatio );
  mCurrentLine.mPoints.append( realPoint );
  if ( mPaths.isEmpty() )
  {
    mPaths.append( mCurrentLine );
    emit newPathAdded();
  }
  else
  {
    mPaths[mPaths.size() - 1] = mCurrentLine;
    emit pathUpdated( {static_cast<int>( mPaths.size() - 1 ) } );
  }
}

void PhotoSketchingController::setActiveColor( const QColor newColor )
{
  mPenColor = newColor;
  mCurrentLine.mColor = newColor;
  if ( !mPaths.isEmpty() )
  {
    mPaths[mPaths.size() - 1] = mCurrentLine;
    emit pathUpdated( {static_cast<int>( mPaths.size() - 1 )} );
  }

  emit activeColorChanged();
}

void PhotoSketchingController::undo()
{
  if ( mPaths.size() > 1 )
  {
    // we remove the last finished path instead of last path as that is the current active path
    mPaths.remove( mPaths.size() - 2 );
    emit lastPathRemoved();
    mPaths[mPaths.size() - 1] = mCurrentLine;
    emit pathUpdated( {static_cast<int>( mPaths.size() - 1 )} );
  }
  if ( mPaths.size() > 1 && !mCanUndo )
  {
    mCanUndo = true;
    emit canUndoChanged();
  }
  else if ( mPaths.size() <= 1 && mCanUndo )
  {
    mCanUndo = false;
    emit canUndoChanged();
  }
}

void PhotoSketchingController::clear()
{
  mPenColor = QColor::fromString( "#FFFFFF" );
  emit activeColorChanged();
  newDrawing();
  mCanUndo = false;
  emit canUndoChanged();
  mPaths.clear();
  emit pathsReset();
}

void PhotoSketchingController::saveDrawings() const
{
  const QString photoPath = QUrl( mPhotoSource ).toLocalFile();
  QImage image( photoPath );
  if ( image.isNull() )
  {
    CoreUtils::log( "Photo annotations", "Failed to load image from: " + photoPath );
    return;
  }

  QPainter painter( &image );
  painter.setRenderHint( QPainter::Antialiasing );

  const int pathCount = mPaths.size();

  for ( int i = 0; i < pathCount; ++i )
  {
    QColor color = mPaths.at( i ).mColor;
    QVector<QPointF> points = mPaths.at( i ).mPoints;

    if ( points.isEmpty() )
      continue;

    // if you are adjusting width here don't forget to adjust it also in MMFormPhotoDrawingPageDialog shapePathComponent
    QPen pen( color, 4 * mPhotoScaleRatio );
    painter.setPen( pen );

    QPainterPath path;
    path.moveTo( points.first().x(),  points.first().y() );
    for ( int j = 1; j < points.size(); ++j )
    {
      path.lineTo( points.at( j ).x(),  points.at( j ).y() );
    }

    painter.drawPath( path );
  }

  if ( !image.save( photoPath ) )
  {
    CoreUtils::log( "Photo annotations", "Failed to save image to: " + photoPath );
  }
  else
  {
    CoreUtils::log( "Photo annotations", "Image saved to: " + photoPath );
  }
}

void PhotoSketchingController::setAnnotationsOffset( const double newXOffset, const double newYOffset )
{
  mAnnotationOffsets.first = newXOffset;
  mAnnotationOffsets.second = newYOffset;
// we want to update all paths
  if ( !mPaths.isEmpty() )
  {
    QVector<int> changedPathIndexes;
    for ( int i = 0; i < mPaths.size() - 1; i++ )
    {
      changedPathIndexes.append( i );
    }
    emit pathUpdated( changedPathIndexes );
  }
}

void PhotoSketchingController::setPhotoScaleRatio( const double newRatio )
{
  mPhotoScaleRatio = newRatio;
// we want to update all paths
  if ( !mPaths.isEmpty() )
  {
    QVector<int> changedPathIndexes;
    for ( int i = 0; i < mPaths.size() - 1; i++ )
    {
      changedPathIndexes.append( i );
    }
    emit pathUpdated( changedPathIndexes );
  }
}

ColorPath PhotoSketchingController::getPath( const int row ) const
{
  if ( row < -mPaths.size() || row >= mPaths.size() ) return ColorPath();

  int index = row;
  if ( row < 0 )
  {
    index = mPaths.size() + row;
  }

  ColorPath colorPath( mPaths.at( index ) );

// we recalculate stored points into the coordinates of printed picture
  QVector<QPointF> shapePoints;
  for ( QPointF point : colorPath.mPoints )
  {
    shapePoints.append( QPointF( ( point.x() / mPhotoScaleRatio ) + mAnnotationOffsets.first, ( point.y() / mPhotoScaleRatio ) + mAnnotationOffsets.second ) );
  }
  colorPath.mPoints = shapePoints;

  return colorPath;
}
