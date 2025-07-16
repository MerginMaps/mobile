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


void PhotoSketchingController::newSketch()
{
  mCurrentLine = ColorPath( mPenColor, {} );
}

void PhotoSketchingController::addPoint( const QPointF newPoint )
{
// we scale up the point to picture's true position
  const QPointF realPoint = QPointF( newPoint.x() * mPhotoScale, newPoint.y() * mPhotoScale );
  mCurrentLine.mPoints.append( realPoint );
  if ( mPaths.isEmpty() || mCurrentLine.mPoints.size() == 1 )
  {
    mPaths.append( mCurrentLine );
    emit newPathAdded( -1 );
    if ( !mCanUndo && !mPaths.isEmpty() )
    {
      mCanUndo = true;
      emit canUndoChanged();
    }
  }
  else
  {
    mPaths[mPaths.size() - 1] = mCurrentLine;
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
  if ( !mPaths.empty() )
  {
    mPaths.removeLast();
    emit lastPathRemoved();
  }

  if ( !mPaths.empty() && !mCanUndo )
  {
    mCanUndo = true;
    emit canUndoChanged();
  }
  else if ( mPaths.empty() && mCanUndo )
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
  mPaths.clear();
  emit pathsReset();
}

void PhotoSketchingController::saveDrawings() const
{
  const QString photoPath = QUrl( mPhotoSource ).toLocalFile();
  QImage image( photoPath );
  if ( image.isNull() )
  {
    CoreUtils::log( "Photo sketching", "Failed to load image from: " + photoPath );
    return;
  }

  QPainter painter( &image );
  painter.setRenderHint( QPainter::Antialiasing );

  const int pathCount = static_cast<int>( mPaths.size() );

  for ( int i = 0; i < pathCount; ++i )
  {
    QColor color = mPaths.at( i ).mColor;
    QVector<QPointF> points = mPaths.at( i ).mPoints;

    if ( points.isEmpty() )
      continue;

    QPen pen( color, SKETCH_WIDTH * mPhotoScale );
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
    CoreUtils::log( "Photo sketching", "Failed to save image to: " + photoPath );
  }
  else
  {
    CoreUtils::log( "Photo sketching", "Image saved to: " + photoPath );
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
  for ( QPointF point : colorPath.mPoints )
  {
    shapePoints.append( QPointF( point.x() / mPhotoScale, point.y() / mPhotoScale ) );
  }
  colorPath.mPoints = shapePoints;

  return colorPath;
}

int PhotoSketchingController::sketchWidth()
{
  return SKETCH_WIDTH;
}
