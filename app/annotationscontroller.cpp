/***************************************************************************
    annotationscontroller.cpp
    ---------------------
    begin                : May 2025
    copyright            : (C) 2025 by Stefanos Natsis
    email                : uclaros at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "annotationscontroller.h"

#include "coreutils.h"
#include "inpututils.h"
#include "inputmapsettings.h"

#include "qgsvectorlayer.h"
#include "qgsvectorlayerutils.h"
#include "qgslinestring.h"
#include "qgsmultilinestring.h"


AnnotationsController::AnnotationsController( QObject *parent )
{
}

AnnotationsController::~AnnotationsController()
{
  if ( mLayer )
    qDebug() << "commit: " << mLayer->commitChanges();
}

void AnnotationsController::updateHighlight( QPointF oldPoint, QPointF newPoint )
{
  if ( mHighlight.isEmpty() )
  {
    const QgsPoint p0 = mMapSettings->screenToCoordinate( oldPoint );
    auto *ls = new QgsLineString( QVector<QgsPoint>() << p0 );
    ls->addZValue();
    ls->addMValue();
    auto *mls = new QgsMultiLineString( QList<QgsLineString *>() << ls );
    mHighlight = QgsGeometry( mls );
    qDebug() << mHighlight.asWkt( 0 );

    mScreenPoints = QgsGeometry( new QgsLineString( { QgsPointXY( oldPoint.x(), oldPoint.y() ) } ) );
  }
  mScreenPoints.insertVertex( newPoint.x(), newPoint.y(), 0 );

  const QgsPoint p1 = mMapSettings->screenToCoordinate( newPoint );
  mHighlight.insertVertex( p1, 0 );


  emit highlightGeometryChanged();
}

void AnnotationsController::finishDigitizing()
{
  qDebug() << "SAVE";
  clearHighlight();


  if ( !mLayer || !mLayer->isValid() )
  {
    CoreUtils::log( QStringLiteral( "Map annotations" ), QStringLiteral( "Can not save map annotation, layer is missing or invalid" ) );
    return;
  }

  if ( mEraserActive )
  {
    QgsVertexIterator it = mScreenPoints.vertices();
    QgsLineString *mapPoints = new QgsLineString;
    while ( it.hasNext() )
    {
      const QgsPoint screenPt = it.next();

      const QgsPoint mapPt = mMapSettings->screenToCoordinate( QPointF( screenPt.x(), screenPt.y() ) );
      mapPoints->addVertex( mapPt );
    }
    const QgsGeometry mapGeom = QgsGeometry( new QgsMultiLineString( QList<QgsLineString *>() << mapPoints ) );
    const QgsGeometry layerGeom = InputUtils::transformGeometry( mapGeom, mMapSettings->destinationCrs(), mLayer );
    QgsFeatureIterator fit = mLayer->getFeatures( QgsFeatureRequest().setNoAttributes().setFilterRect( layerGeom.boundingBox() ) );
    QgsFeature f;
    QgsFeatureIds fids;
    while ( fit.nextFeature( f ) )
    {
      if ( layerGeom.intersects( f.geometry() ) )
        fids.insert( f.id() );
    }

    if ( !fids.isEmpty() )
    {
      mLayer->beginEditCommand( QStringLiteral( "Delete map annotation" ) );
      mLayer->deleteFeatures( fids );
      mLayer->endEditCommand();
    }
  }
  else
  {
    mScreenPoints = mScreenPoints.simplify( 5 );
    QgsVertexIterator it = mScreenPoints.vertices();
    QgsLineString *simplifiedMapPoints = new QgsLineString;
    while ( it.hasNext() )
    {
      QgsPoint screenPt = it.next();

      QgsPoint mapPt = mMapSettings->screenToCoordinate( QPointF( screenPt.x(), screenPt.y() ) );
      mapPt.addZValue();
      mapPt.addMValue();
      simplifiedMapPoints->addVertex( mapPt );
    }
    const QgsGeometry multiLineGeom = QgsGeometry( new QgsMultiLineString( QList<QgsLineString *>() << simplifiedMapPoints ) );

    const QgsGeometry geom = InputUtils::transformGeometry( multiLineGeom, mMapSettings->destinationCrs(), mLayer );
    QgsFeature feature = QgsVectorLayerUtils::createFeature( mLayer, geom );
    feature.setAttribute( QStringLiteral( "color" ), mColor );
    mLayer->beginEditCommand( QStringLiteral( "Add map annotation" ) );
    mLayer->addFeature( feature );
    mLayer->endEditCommand();
  }
}

void AnnotationsController::undo()
{
  if ( !mLayer )
    return;

  mLayer->undoStack()->undo();
}

QgsGeometry AnnotationsController::highlightGeometry() const
{
  return mHighlight;
}

QStringList AnnotationsController::availableColors() const
{
  QgsProject *project = mMapSettings->project();

  const QStringList defaultColors = { "#FFFFFF", "#12181F", "#5E9EE4", "#57B46F", "#FDCB2A", "#FF9C40", "#FF8F93" };
  return project->readListEntry( QStringLiteral( "Mergin" ), QStringLiteral( "MapAnnotations/Colors" ), defaultColors );
}

void AnnotationsController::clearHighlight()
{
  mHighlight = QgsGeometry( new QgsMultiLineString() );
  emit highlightGeometryChanged();
}

void AnnotationsController::setMapSettings( InputMapSettings *settings )
{
  if ( !settings )
    return;

  QgsProject *project = settings->project();

  if ( !project )
  {
    CoreUtils::log( QStringLiteral( "Map annotations" ), QStringLiteral( "Can not save map annotation, missing required properties" ) );
    return;
  }

  clearHighlight();

  const QString layerId = project->readEntry( QStringLiteral( "Mergin" ), QStringLiteral( "MapAnnotations/Layer" ) );
  mLayer = project->mapLayer<QgsVectorLayer *>( layerId );

  if ( !mLayer )
  {
    CoreUtils::log( QStringLiteral( "Map annotations" ), QStringLiteral( "Can not initialize map annotations, layer is missing" ) );
  }
  else
  {
    mLayer->startEditing();
    connect( mLayer->undoStack(), &QUndoStack::canUndoChanged, this, &AnnotationsController::canUndoChanged );
  }

  mMapSettings = settings;
  emit mapSettingsChanged();
}

InputMapSettings *AnnotationsController::mapSettings() const
{
  return mMapSettings;
}

bool AnnotationsController::canUndo() const
{
  if ( !mLayer )
    return false;

  return mLayer->undoStack()->canUndo();
}

bool AnnotationsController::eraserActive() const
{
  return mEraserActive;
}

void AnnotationsController::setEraserActive( bool active )
{
  if ( active == mEraserActive )
    return;

  mEraserActive = active;
  emit eraserActiveChanged();
}

void AnnotationsController::setActiveColor( const QColor &color )
{
  if ( color == mColor )
    return;

  mColor = color;
  emit activeColorChanged();
}

QColor AnnotationsController::activeColor() const
{
  return mColor;
}
