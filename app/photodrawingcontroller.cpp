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
#include <QPainterPath>
#include <QPainter>
#include <QUrl>

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
  if ( !mCanUndo && mColorPathModel->rowCount() > 1 )
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
 if ( mColorPathModel->rowCount() == 0 )
 {
  mColorPathModel->addPath( mCurrentLine );
 } else
 {
  mColorPathModel->updatePath( mColorPathModel->rowCount() - 1, mCurrentLine );
 }
 if ( xOffset != mAnnotationOffsets.first || yOffset != mAnnotationOffsets.second )
 {
  mAnnotationOffsets.first = xOffset;
  mAnnotationOffsets.second = yOffset;
 }
}

void PhotoDrawingController::setActiveColor(const QColor newColor)
{
 CoreUtils::log( "Photo annotations", "Setting current color to " + newColor.name() );
 mPenColor = newColor;
 mCurrentLine.mColor = newColor;
 if ( mColorPathModel->rowCount() > 0 )
 {
  mColorPathModel->updatePath( mColorPathModel->rowCount() - 1, mCurrentLine );
 }

 emit activeColorChanged();
}

void PhotoDrawingController::undo()
{
 CoreUtils::log( "Photo annotations", "Removing the last line");
 if ( mColorPathModel->rowCount() > 1 )
 {
  CoreUtils::log( "Photo annotations", "Removed the last line saved");
  mColorPathModel->removeLastPath();
  mColorPathModel->updatePath( mColorPathModel->rowCount() - 1, mCurrentLine );
  emit annotationsChanged();
 }
 if ( mColorPathModel->rowCount() > 1 && !mCanUndo )
 {
  mCanUndo = true;
  emit canUndoChanged();
 } else if ( mColorPathModel->rowCount() <= 1 && mCanUndo )
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
 const QString photoPath = QUrl(mPhotoSource).toLocalFile();
 QImage image(photoPath);
 if (image.isNull()) {
  CoreUtils::log( "Photo annotations", "Failed to load image from: " + photoPath );
  return;
 }
 CoreUtils::log( "Photo annotations", "Image size: [" + QString::number(image.width()) + ", " + QString::number(image.height()) + "]" );
 CoreUtils::log( "Photo annotations", "Photo scale ratio: " + QString::number(mPhotoScaleRatio) );

 QPainter painter(&image);
 painter.setRenderHint(QPainter::Antialiasing);

 const int rowCount = mColorPathModel->rowCount();
 const qreal offsetX = mAnnotationOffsets.first;
 const qreal offsetY = mAnnotationOffsets.second;

 for (int i = 0; i < rowCount; ++i) {
  QVariant pathVariant = mColorPathModel->getPath(i);
  QVariantMap pathMap = pathVariant.toMap();

  QColor color = pathMap["color"].value<QColor>();
  QVector<QPointF> points = pathMap["points"].value<QVector<QPointF>>();

  if (points.isEmpty())
   continue;

  // if you are adjusting width here don't forget to adjust it also in MMFormPhotoDrawingPageDialog shapePathComponent
  QPen pen(color, 2 * mPhotoScaleRatio );
  CoreUtils::log( "Photo annotations", "Pen color: " + pen.color().name() );
  painter.setPen(pen);

  // the reason for the recalculation of positions: the path is stored in ColorPathModel which is directly used by
  // ShapePath, thus the coordinates are moved by offset ( to take in mind the padding around the photo ) and they are
  // scaled to the painted image size, however here we need to scale back to the real size of the photo
  QPainterPath path;
  path.moveTo( ( points.first().x() - offsetX ) * mPhotoScaleRatio, ( points.first().y() - offsetY ) * mPhotoScaleRatio );
  CoreUtils::log( "Photo annotations", "Starting position of path: [" + QString::number(path.currentPosition().x()) + ", " + QString::number(path.currentPosition().y()) + "]" );
  for (int j = 1; j < points.size(); ++j) {
   path.lineTo( ( points[j].x() - offsetX ) * mPhotoScaleRatio, ( points[j].y() - offsetY ) * mPhotoScaleRatio );
   CoreUtils::log( "Photo annotations", "Current position of path: [" + QString::number(path.currentPosition().x()) + ", " + QString::number(path.currentPosition().y()) + "]" );
  }

  painter.drawPath(path);
 }

 if (!image.save(photoPath)) {
  CoreUtils::log( "Photo annotations", "Failed to save image to: " + photoPath );
 } else {
  CoreUtils::log( "Photo annotations", "Image saved to: " + photoPath );
 }
}

void PhotoDrawingController::setPhotoScaleRatio( const double ratio)
{
 mPhotoScaleRatio = ratio;
}

ColorPathModel *PhotoDrawingController::annotations() const
{
 CoreUtils::log( "Photo annotations", "Requesting annotations. Count: " + QString::number( mColorPathModel->rowCount() ) );
 return mColorPathModel;
}
