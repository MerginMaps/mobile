/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "mapsketchingcontroller.h"

#include "coreutils.h"
#include "inpututils.h"
#include "inputmapsettings.h"

#include "qgsvectorlayer.h"
#include "qgsvectorlayerutils.h"
#include "qgslinestring.h"
#include "qgsmultilinestring.h"


MapSketchingController::MapSketchingController( QObject *parent )
  : QObject( parent )
{
}

MapSketchingController::~MapSketchingController()
{
  if ( !mLayer )
    return;

  if ( !mLayer->commitChanges() )
  {
    CoreUtils::log( QStringLiteral( "Map sketching" ), QStringLiteral( "Could not save changes to map sketching layer" ) );
  }
}

void MapSketchingController::updateHighlight( const QPointF &oldPoint, const QPointF &newPoint )
{
  if ( mHighlight.isEmpty() )
  {
    const QgsPoint p0 = mMapSettings->screenToCoordinate( oldPoint );
    QgsLineString *ls = new QgsLineString( QVector<QgsPoint>() << p0 );
    ls->addZValue();
    ls->addMValue();
    QgsMultiLineString *mls = new QgsMultiLineString( QList<QgsLineString *>() << ls );
    mHighlight = QgsGeometry( mls );

    mScreenPoints = QgsGeometry( new QgsLineString( { QgsPointXY( oldPoint.x(), oldPoint.y() ) } ) );
  }

  // TODO: append instead of insert to zero
  mScreenPoints.insertVertex( newPoint.x(), newPoint.y(), 0 );
  const QgsPoint p1 = mMapSettings->screenToCoordinate( newPoint );
  mHighlight.insertVertex( p1, 0 );

  emit highlightGeometryChanged();
}

void MapSketchingController::finishDigitizing()
{
  clearHighlight();


  if ( !mLayer || !mLayer->isValid() )
  {
    CoreUtils::log( QStringLiteral( "Map sketching" ), QStringLiteral( "Can not save map sketches, layer is missing or invalid" ) );
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
      mLayer->beginEditCommand( QStringLiteral( "Delete map sketches" ) );
      mLayer->deleteFeatures( fids );
      mLayer->endEditCommand();
    }
  }
  else
  {
    mScreenPoints = mScreenPoints.simplify( 1 );
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
    // TODO: Variable Manager for expressions?
    mLayer->beginEditCommand( QStringLiteral( "Add map sketches" ) );
    mLayer->addFeature( feature );
    mLayer->endEditCommand();
  }
}

void MapSketchingController::redo() const
{
  if ( !mLayer )
    return;

  mLayer->undoStack()->redo();
}

void MapSketchingController::undo() const
{
  if ( !mLayer )
    return;

  mLayer->undoStack()->undo();
}

QgsGeometry MapSketchingController::highlightGeometry() const
{
  return mHighlight;
}

QStringList MapSketchingController::availableColors() const
{
  const QStringList defaultColors = { "#FFFFFF", "#12181F", "#5E9EE4", "#57B46F", "#FDCB2A", "#FF9C40", "#FF8F93" };
  return mMapSettings->project()->readListEntry( QStringLiteral( "Mergin" ), QStringLiteral( "MapSketching/Colors" ), defaultColors );
}

void MapSketchingController::clearHighlight()
{
  mHighlight = QgsGeometry( new QgsMultiLineString() );
  emit highlightGeometryChanged();
}

void MapSketchingController::setMapSettings( InputMapSettings *settings )
{
  if ( !settings )
    return;

  const QgsProject *project = settings->project();

  if ( !project )
  {
    CoreUtils::log( QStringLiteral( "Map sketches" ), QStringLiteral( "Can not save map sketches, missing required properties" ) );
    return;
  }

  clearHighlight();

  const QString layerId = project->readEntry( QStringLiteral( "Mergin" ), QStringLiteral( "MapSketching/Layer" ) );
  mLayer = project->mapLayer<QgsVectorLayer *>( layerId );

  if ( !mLayer )
  {
    CoreUtils::log( QStringLiteral( "Map sketches" ), QStringLiteral( "Can not initialize map sketches, layer %1 is missing" ).arg( layerId ) );
  }
  else
  {
    mLayer->startEditing();
    connect( mLayer->undoStack(), &QUndoStack::canUndoChanged, this, &MapSketchingController::canUndoChanged );
    connect( mLayer->undoStack(), &QUndoStack::canRedoChanged, this, &MapSketchingController::canRedoChanged );
  }

  mMapSettings = settings;
  emit mapSettingsChanged();
}

InputMapSettings *MapSketchingController::mapSettings() const
{
  return mMapSettings;
}

bool MapSketchingController::canRedo() const
{
  if ( !mLayer )
    return false;

  return mLayer->undoStack()->canRedo();
}

bool MapSketchingController::canUndo() const
{
  if ( !mLayer )
    return false;

  return mLayer->undoStack()->canUndo();
}

bool MapSketchingController::eraserActive() const
{
  return mEraserActive;
}

void MapSketchingController::setEraserActive( bool active )
{
  if ( active == mEraserActive )
    return;

  mEraserActive = active;
  emit eraserActiveChanged();
}

void MapSketchingController::setActiveColor( const QColor &color )
{
  if ( color == mColor )
    return;

  mColor = color;
  emit activeColorChanged();
}

QColor MapSketchingController::activeColor() const
{
  return mColor;
}
