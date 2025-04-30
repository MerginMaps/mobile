/**************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * The source code forked from https://github.com/qgis/QGIS on 25th Nov 2022
 * File: qgsquickmapsettings.cpp by (C) 2014 by Matthias Kuhn
 */

#include "qgis.h"
#include "inputmapsettings.h"
#include "coreutils.h"

#include "qgsmaplayer.h"
#include "qgsmessagelog.h"
#include "qgsprojectviewsettings.h"

static constexpr int EXTENT_SAVE_DELAY_MS = 2000;

InputMapSettings::InputMapSettings( QObject *parent )
  : QObject( parent )
{
  // Connect signals for derived values
  connect( this, &InputMapSettings::destinationCrsChanged, this, &InputMapSettings::mapUnitsPerPixelChanged );
  connect( this, &InputMapSettings::extentChanged, this, &InputMapSettings::mapUnitsPerPixelChanged );
  connect( this, &InputMapSettings::outputSizeChanged, this, &InputMapSettings::mapUnitsPerPixelChanged );
  connect( this, &InputMapSettings::extentChanged, this, &InputMapSettings::visibleExtentChanged );
  connect( this, &InputMapSettings::rotationChanged, this, &InputMapSettings::visibleExtentChanged );
  connect( this, &InputMapSettings::outputSizeChanged, this, &InputMapSettings::visibleExtentChanged );

  // store map extent to QSettings when extent hasn't changed
  mSaveExtentTimer.setSingleShot( true );
  connect( &mSaveExtentTimer, &QTimer::timeout, this, &InputMapSettings::saveExtentToSettings );
  connect( this, &InputMapSettings::extentChanged, this, [this]() { mSaveExtentTimer.start( EXTENT_SAVE_DELAY_MS ); } );
}

void InputMapSettings::setProject( QgsProject *project )
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
    connect( mProject, &QgsProject::readProject, this, &InputMapSettings::onReadProject );
    connect( mProject, &QgsProject::crsChanged, this, &InputMapSettings::onCrsChanged );
    setDestinationCrs( mProject->crs() );
    mMapSettings.setTransformContext( mProject->transformContext() );
    mMapSettings.setPathResolver( mProject->pathResolver() );
  }
  else
  {
    mMapSettings.setTransformContext( QgsCoordinateTransformContext() );
  }

  emit projectChanged();
}

QgsProject *InputMapSettings::project() const
{
  return mProject;
}

QgsCoordinateTransformContext InputMapSettings::transformContext() const
{
  return mMapSettings.transformContext();
}

QgsRectangle InputMapSettings::extent() const
{
  return mMapSettings.extent();
}

void InputMapSettings::setExtent( const QgsRectangle &extent )
{
  if ( mMapSettings.extent() == extent )
    return;

  mMapSettings.setExtent( extent );
  emit extentChanged();
}

QgsPoint InputMapSettings::center() const
{
  return QgsPoint( extent().center() );
}

void InputMapSettings::setCenter( const QgsPoint &center )
{
  QgsVector delta = QgsPointXY( center ) - mMapSettings.extent().center();

  QgsRectangle e = mMapSettings.extent();
  e.setXMinimum( e.xMinimum() + delta.x() );
  e.setXMaximum( e.xMaximum() + delta.x() );
  e.setYMinimum( e.yMinimum() + delta.y() );
  e.setYMaximum( e.yMaximum() + delta.y() );

  setExtent( e );
}

double InputMapSettings::mapUnitsPerPixel() const
{
  return mMapSettings.mapUnitsPerPixel();
}

void InputMapSettings::setCenterToLayer( QgsMapLayer *layer, bool shouldZoom )
{
  Q_ASSERT( layer );

  const QgsRectangle extent = mapSettings().layerToMapCoordinates( layer, layer->extent() );

  if ( !extent.isEmpty() )
  {
    if ( shouldZoom )
      setExtent( extent );
    else
      setCenter( QgsPoint( extent.center() ) );
  }
}

double InputMapSettings::mapUnitsPerPoint() const
{
  return mMapSettings.mapUnitsPerPixel() * devicePixelRatio();
}

QgsRectangle InputMapSettings::visibleExtent() const
{
  return mMapSettings.visibleExtent();
}

QPointF InputMapSettings::coordinateToScreen( const QgsPoint &point ) const
{
  QgsPointXY pt( point.x(), point.y() );
  QgsPointXY pp = mMapSettings.mapToPixel().transform( pt );
  pp.setX( pp.x() / devicePixelRatio() );
  pp.setY( pp.y() / devicePixelRatio() );
  return pp.toQPointF();
}

QgsPoint InputMapSettings::screenToCoordinate( const QPointF &point ) const
{
  const QgsPointXY pp = mMapSettings.mapToPixel().toMapCoordinates( point.x() * devicePixelRatio(), point.y() * devicePixelRatio() );
  return QgsPoint( pp );
}

void InputMapSettings::setTransformContext( const QgsCoordinateTransformContext &ctx )
{
  mMapSettings.setTransformContext( ctx );
}

QgsMapSettings InputMapSettings::mapSettings() const
{
  return mMapSettings;
}

QSize InputMapSettings::outputSize() const
{
  return mMapSettings.outputSize();
}

void InputMapSettings::setOutputSize( QSize outputSize )
{
  outputSize.setWidth( outputSize.width() * devicePixelRatio() );
  outputSize.setHeight( outputSize.height() * devicePixelRatio() );
  if ( mMapSettings.outputSize() == outputSize )
    return;

  mMapSettings.setOutputSize( outputSize );
  emit outputSizeChanged();
}

double InputMapSettings::outputDpi() const
{
  return mMapSettings.outputDpi();
}

void InputMapSettings::setOutputDpi( double outputDpi )
{
  outputDpi *= devicePixelRatio();
  if ( qgsDoubleNear( mMapSettings.outputDpi(), outputDpi ) )
    return;

  mMapSettings.setOutputDpi( outputDpi );
  emit outputDpiChanged();
}

QgsCoordinateReferenceSystem InputMapSettings::destinationCrs() const
{
  return mMapSettings.destinationCrs();
}

void InputMapSettings::setDestinationCrs( const QgsCoordinateReferenceSystem &destinationCrs )
{
  if ( mMapSettings.destinationCrs() == destinationCrs )
    return;

  mMapSettings.setDestinationCrs( destinationCrs );
  emit destinationCrsChanged();
}

QList<QgsMapLayer *> InputMapSettings::layers() const
{
  return mMapSettings.layers();
}

void InputMapSettings::setLayers( const QList<QgsMapLayer *> &layers )
{
  mMapSettings.setLayers( layers );
  emit layersChanged();
}

void InputMapSettings::onCrsChanged()
{
  setDestinationCrs( mProject->crs() );
}

void InputMapSettings::onReadProject( const QDomDocument &doc )
{
  if ( mProject )
  {
    int red = mProject->readNumEntry( QStringLiteral( "Gui" ), QStringLiteral( "/CanvasColorRedPart" ), 255 );
    int green = mProject->readNumEntry( QStringLiteral( "Gui" ), QStringLiteral( "/CanvasColorGreenPart" ), 255 );
    int blue = mProject->readNumEntry( QStringLiteral( "Gui" ), QStringLiteral( "/CanvasColorBluePart" ), 255 );
    mMapSettings.setBackgroundColor( QColor( red, green, blue ) );

    const bool isTemporal = mProject->readNumEntry( QStringLiteral( "TemporalControllerWidget" ), QStringLiteral( "/NavigationMode" ), 0 ) != 0;
    const QString startString = QgsProject::instance()->readEntry( QStringLiteral( "TemporalControllerWidget" ), QStringLiteral( "/StartDateTime" ) );
    const QString endString = QgsProject::instance()->readEntry( QStringLiteral( "TemporalControllerWidget" ), QStringLiteral( "/EndDateTime" ) );
    mMapSettings.setIsTemporal( isTemporal );
    mMapSettings.setTemporalRange( QgsDateTimeRange( QDateTime::fromString( startString, Qt::ISODateWithMs ),
                                   QDateTime::fromString( endString, Qt::ISODateWithMs ) ) );
  }

  QDomNodeList nodes = doc.elementsByTagName( "mapcanvas" );
  bool foundTheMapCanvas = false;
  for ( int i = 0; i < nodes.size(); i++ )
  {
    QDomNode node = nodes.item( 0 );
    QDomElement element = node.toElement();

    if ( element.hasAttribute( QStringLiteral( "name" ) ) && element.attribute( QStringLiteral( "name" ) ) == QLatin1String( "theMapCanvas" ) )
    {
      foundTheMapCanvas = true;
      mMapSettings.readXml( node );

      if ( !qgsDoubleNear( mMapSettings.rotation(), 0 ) )
        QgsMessageLog::logMessage( QStringLiteral( "Map Canvas rotation is not supported. Resetting from %1 to 0." ).arg( mMapSettings.rotation() ) );
    }
  }
  if ( !foundTheMapCanvas )
  {
    mMapSettings.setDestinationCrs( mProject->crs() );
    mMapSettings.setExtent( mProject->viewSettings()->fullExtent() );
  }

  loadSavedExtent();

  mMapSettings.setRotation( 0 );

  mMapSettings.setTransformContext( mProject->transformContext() );
  mMapSettings.setPathResolver( mProject->pathResolver() );

  emit extentChanged();
  emit destinationCrsChanged();
  emit outputSizeChanged();
  emit outputDpiChanged();
  emit layersChanged();
  emit temporalStateChanged();
}

double InputMapSettings::rotation() const
{
  return mMapSettings.rotation();
}

void InputMapSettings::setRotation( double rotation )
{
  if ( !qgsDoubleNear( rotation, 0 ) )
    QgsMessageLog::logMessage( QStringLiteral( "Map Canvas rotation is not supported. Resetting from %1 to 0." ).arg( rotation ) );
}

QColor InputMapSettings::backgroundColor() const
{
  return mMapSettings.backgroundColor();
}

void InputMapSettings::setBackgroundColor( const QColor &color )
{
  if ( mMapSettings.backgroundColor() == color )
    return;

  mMapSettings.setBackgroundColor( color );
  emit backgroundColorChanged();
}

qreal InputMapSettings::devicePixelRatio() const
{
  return mDevicePixelRatio;
}

void InputMapSettings::setDevicePixelRatio( const qreal &devicePixelRatio )
{
  mDevicePixelRatio = devicePixelRatio;
  emit devicePixelRatioChanged();
}

QgsPoint InputMapSettings::toQgsPoint( const QPointF &point ) const
{
  return QgsPoint( point );
}

bool InputMapSettings::isTemporal() const
{
  return mMapSettings.isTemporal();
}

void InputMapSettings::setIsTemporal( bool temporal )
{
  mMapSettings.setIsTemporal( temporal );
  emit temporalStateChanged();
}

QDateTime InputMapSettings::temporalBegin() const
{
  return mMapSettings.temporalRange().begin();
}

void InputMapSettings::setTemporalBegin( const QDateTime &begin )
{
  const QgsDateTimeRange range = mMapSettings.temporalRange();
  mMapSettings.setTemporalRange( QgsDateTimeRange( begin, range.end() ) );
  emit temporalStateChanged();
}

QDateTime InputMapSettings::temporalEnd() const
{
  return mMapSettings.temporalRange().end();
}

void InputMapSettings::setTemporalEnd( const QDateTime &end )
{
  const QgsDateTimeRange range = mMapSettings.temporalRange();
  mMapSettings.setTemporalRange( QgsDateTimeRange( range.begin(), end ) );
  emit temporalStateChanged();
}

QString InputMapSettings::projectId() const
{
  return mProjectId;
}

void InputMapSettings::setProjectId( const QString &projectId )
{
  if ( projectId == mProjectId )
    return;

  mProjectId = projectId;
  emit projectIdChanged();
}

void InputMapSettings::saveExtentToSettings()
{
  QSettings settings;
  const QgsRectangle extent = this->extent();
  if ( !extent.isEmpty() && extent.isFinite() && !mProjectId.isEmpty() )
  {
    settings.beginGroup( QStringLiteral( "%1/%2" ).arg( mProjectId, CoreUtils::QSETTINGS_CACHED_MAP_EXTENT_GROUP ) );
    settings.setValue( "extent", extent );
    settings.endGroup();
  }
}

void InputMapSettings::loadSavedExtent()
{
  QSettings settings;
  settings.beginGroup( QStringLiteral( "%1/%2" ).arg( mProjectId, CoreUtils::QSETTINGS_CACHED_MAP_EXTENT_GROUP ) );
  QgsRectangle extent = settings.value( "extent" ).value<QgsRectangle>();
  settings.endGroup();

  if ( !extent.isEmpty() && extent.isFinite() && !mProjectId.isEmpty() )
  {
    setExtent( extent );
  }
}

