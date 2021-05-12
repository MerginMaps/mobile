/***************************************************************************
  mapsettings.cpp
  --------------------------------------
  Date                 : 27.12.2014
  Copyright            : (C) 2014 by Matthias Kuhn
  Email                : matthias (at) opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "qgsmaplayer.h"
#include "qgsmaplayerstylemanager.h"
#include "qgsmessagelog.h"
#include "qgsproject.h"
#include "qgis.h"

#include "mapsettings.h"

MapSettings::MapSettings( QObject *parent )
  : QObject( parent )
{
  // Connect signals for derived values
  connect( this, &MapSettings::destinationCrsChanged, this, &MapSettings::mapUnitsPerPixelChanged );
  connect( this, &MapSettings::extentChanged, this, &MapSettings::mapUnitsPerPixelChanged );
  connect( this, &MapSettings::outputSizeChanged, this, &MapSettings::mapUnitsPerPixelChanged );
  connect( this, &MapSettings::extentChanged, this, &MapSettings::visibleExtentChanged );
  connect( this, &MapSettings::rotationChanged, this, &MapSettings::visibleExtentChanged );
  connect( this, &MapSettings::outputSizeChanged, this, &MapSettings::visibleExtentChanged );
}

void MapSettings::setProject( QgsProject *project )
{
  if ( project == mProject )
    return;

  // If we have already something connected, disconnect it!
  if ( mProject )
  {
    mProject->disconnect( this );
  }

  mProject = project;

  // Connect all signals
  if ( mProject )
  {
    connect( mProject, &QgsProject::readProject, this, &MapSettings::onReadProject );
    setDestinationCrs( mProject->crs() );
    mMapSettings.setTransformContext( mProject->transformContext() );
  }
  else
  {
    mMapSettings.setTransformContext( QgsCoordinateTransformContext() );
  }

  emit projectChanged();
}

QgsProject *MapSettings::project() const
{
  return mProject;
}

QgsCoordinateTransformContext MapSettings::transformContext() const
{
  return mMapSettings.transformContext();
}

QgsRectangle MapSettings::extent() const
{
  return mMapSettings.extent();
}

void MapSettings::setExtent( const QgsRectangle &extent )
{
  if ( mMapSettings.extent() == extent )
    return;

  mMapSettings.setExtent( extent );
  emit extentChanged();
}

void MapSettings::setCenter( const QgsPoint &center )
{
  QgsVector delta = QgsPointXY( center ) - mMapSettings.extent().center();

  QgsRectangle e = mMapSettings.extent();
  e.setXMinimum( e.xMinimum() + delta.x() );
  e.setXMaximum( e.xMaximum() + delta.x() );
  e.setYMinimum( e.yMinimum() + delta.y() );
  e.setYMaximum( e.yMaximum() + delta.y() );

  setExtent( e );
}

double MapSettings::mapUnitsPerPixel() const
{
  return mMapSettings.mapUnitsPerPixel();
}

QgsRectangle MapSettings::visibleExtent() const
{
  return mMapSettings.visibleExtent();
}

QPointF MapSettings::coordinateToScreen( const QgsPoint &point ) const
{
  QgsPointXY pt( point.x(), point.y() );
  QgsPointXY pp = mMapSettings.mapToPixel().transform( pt );
  return pp.toQPointF();
}

QgsPoint MapSettings::screenToCoordinate( const QPointF &point ) const
{
  // use floating point precision with mapToCoordinates (i.e. do not use QPointF::toPoint)
  // this is to avoid rounding errors with an odd screen width or height
  // and the point being set to the exact center of it
  const QgsPointXY pp = mMapSettings.mapToPixel().toMapCoordinates( point.x(), point.y() );
  return QgsPoint( pp );
}

QgsMapSettings MapSettings::mapSettings() const
{
  return mMapSettings;
}

void MapSettings::setTransformContext( const QgsCoordinateTransformContext &ctx )
{
  mMapSettings.setTransformContext( ctx );
}

QSize MapSettings::outputSize() const
{
  return mMapSettings.outputSize();
}

void MapSettings::setOutputSize( const QSize &outputSize )
{
  if ( mMapSettings.outputSize() == outputSize )
    return;

  mMapSettings.setOutputSize( outputSize );
  emit outputSizeChanged();
}

double MapSettings::outputDpi() const
{
  return mMapSettings.outputDpi();
}

void MapSettings::setOutputDpi( double outputDpi )
{
  if ( qgsDoubleNear( mMapSettings.outputDpi(), outputDpi ) )
    return;

  mMapSettings.setOutputDpi( outputDpi );
  emit outputDpiChanged();
}

QgsCoordinateReferenceSystem MapSettings::destinationCrs() const
{
  return mMapSettings.destinationCrs();
}

void MapSettings::setDestinationCrs( const QgsCoordinateReferenceSystem &destinationCrs )
{
  if ( mMapSettings.destinationCrs() == destinationCrs )
    return;

  mMapSettings.setDestinationCrs( destinationCrs );
  emit destinationCrsChanged();
}

QList<QgsMapLayer *> MapSettings::layers() const
{
  return mMapSettings.layers();
}

void MapSettings::setLayers( const QList<QgsMapLayer *> &layers )
{
  mMapSettings.setLayers( layers );
  emit layersChanged();
}

void MapSettings::onReadProject( const QDomDocument &doc )
{
  if ( mProject )
  {
    mMapSettings.setBackgroundColor( mProject->backgroundColor() );
  }

  QDomNodeList nodes = doc.elementsByTagName( "mapcanvas" );
  if ( nodes.count() )
  {
    QDomNode node = nodes.item( 0 );

    mMapSettings.readXml( node );

    if ( !qgsDoubleNear( mMapSettings.rotation(), 0 ) )
      QgsMessageLog::logMessage( tr( "Map Canvas rotation is not supported. Resetting from %1 to 0." ).arg( mMapSettings.rotation() ) );

    mMapSettings.setRotation( 0 );

    emit extentChanged();
    emit destinationCrsChanged();
    emit outputSizeChanged();
    emit outputDpiChanged();
    emit layersChanged();
  }
}

double MapSettings::rotation() const
{
  return mMapSettings.rotation();
}

void MapSettings::setRotation( double rotation )
{
  if ( !qgsDoubleNear( rotation, 0 ) )
    QgsMessageLog::logMessage( tr( "Map Canvas rotation is not supported. Resetting from %1 to 0." ).arg( rotation ) );
}

QColor MapSettings::backgroundColor() const
{
  return mMapSettings.backgroundColor();
}

void MapSettings::setBackgroundColor( const QColor &color )
{
  if ( mMapSettings.backgroundColor() == color )
    return;

  mMapSettings.setBackgroundColor( color );
  emit backgroundColorChanged();
}
