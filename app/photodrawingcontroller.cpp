/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "photodrawingcontroller.h"

#include <QColor>
#include <QPointF>

#include "coreutils.h"

PhotoDrawingController::PhotoDrawingController( QObject *parent ): QObject( parent )
{
 mColorPathModel = new ColorPathModel( this );
};

void PhotoDrawingController::newDrawing()
{
 CoreUtils::log( "Photo annotations", "Creating new line to draw");
 if ( !mCurrentLine.mPoints.isEmpty() )
 {
  mColorPathModel->addPath( mCurrentLine );
  emit annotationsChanged();
  if ( !mCanUndo )
  {
   mCanUndo = true;
   emit canUndoChanged();
  }
 }
 mCurrentLine = ColorPath( mPenColor, {} );
}

void PhotoDrawingController::addPoint(const QPointF newPoint, const double xOffset, const double yOffset)
{
 const QPointF offsetPoint = QPointF( newPoint.x() + xOffset, newPoint.y() + yOffset );
 CoreUtils::log("Photo annotations", QString().append("Adding new point (").append(
                 QString::number(offsetPoint.x()).append(", ").append(QString::number(offsetPoint.y())).append(
                  ") to current line")));
 mCurrentLine.mPoints.append( offsetPoint );
}

void PhotoDrawingController::setActiveColor(const QColor newColor)
{
 CoreUtils::log( "Photo annotations", "Setting current color to " + newColor.name() );
 mPenColor = newColor;
 mCurrentLine.mColor = newColor;

 emit activeColorChanged();
}

void PhotoDrawingController::undo()
{
 CoreUtils::log( "Photo annotations", "Removing the last line");
 if ( !mColorPathModel->isEmpty() )
 {
  CoreUtils::log( "Photo annotations", "Removed the last line saved");
  mColorPathModel->removeLastPath();
  emit annotationsChanged();
 }
 if ( !mColorPathModel->isEmpty() && !mCanUndo )
 {
  mCanUndo = true;
  emit canUndoChanged();
 } else if ( mColorPathModel->isEmpty() && mCanUndo )
 {
  mCanUndo = false;
  emit canUndoChanged();
 }
}

void PhotoDrawingController::clear()
{
 mPenColor = QColor::fromString("#FFFFFF");
 emit activeColorChanged();
 newDrawing();
 mCanUndo = false;
 emit canUndoChanged();
 mColorPathModel->clear();
 emit annotationsChanged();
}

void PhotoDrawingController::saveDrawings()
{
 CoreUtils::log( "Photo annotations", "I'm not empty!");
}

ColorPathModel *PhotoDrawingController::annotations() const
{
 CoreUtils::log( "Photo annotations", "Requesting annotations. Count: " + QString::number( mColorPathModel->rowCount() ) );
 return mColorPathModel;
}
