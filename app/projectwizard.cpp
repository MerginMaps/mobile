/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "projectwizard.h"
#include "inpututils.h"
#include "coreutils.h"

#include "qgsproject.h"
#include "qgsvectortilelayer.h"
#include "qgsvectorlayer.h"
#include "qgsvectorfilewriter.h"
#include "qgsdatetimefieldformatter.h"
#include "qgsmarkersymbollayer.h"
#include "qgis.h"
#include "qgslinesymbol.h"
#include "qgssymbollayer.h"
#include "qgssymbollayerutils.h"
#include "qgssymbol.h"
#include "qgsmarkersymbol.h"
#include "qgssinglesymbolrenderer.h"
#include "inpututils.h"
#include "coreutils.h"

const QString TILES_URL = QStringLiteral( "https://tiles.merginmaps.com" );

ProjectWizard::ProjectWizard( const QString &dataDir, QObject *parent )
  : QObject( parent )
  , mDataDir( dataDir )
{

  mSettings = std::unique_ptr<QgsMapSettings>( new QgsMapSettings );
}

QgsVectorLayer *ProjectWizard::createGpkgLayer( QString const &projectDir, QList<FieldConfiguration> const &fieldsConfig )
{
  QString gpkgName( QStringLiteral( "data" ) );
  QString projectGpkgPath( QString( "%1/%2.%3" ).arg( projectDir ).arg( gpkgName ).arg( "gpkg" ) );
  QString layerName( QStringLiteral( "Survey" ) );
  QgsCoordinateReferenceSystem layerCrs( LAYER_CRS_ID );
  QgsFields predefinedFields = createFields( fieldsConfig );

  // Write layer as gpkg
  QgsVectorLayer *layer = new QgsVectorLayer( QStringLiteral( "PointZ?crs=%1" ).arg( LAYER_CRS_ID ), layerName, "memory" );
  layer->startEditing();
  layer->setCrs( layerCrs );
  for ( QgsField f : predefinedFields )
  {
    layer->addAttribute( f );
  }
  layer->updateFields();
  layer->commitChanges();

  QgsVectorFileWriter::SaveVectorOptions options;
  options.driverName = QStringLiteral( "GPKG" );
  options.layerName = layerName;
  options.fileEncoding = QStringLiteral( "UTF-8" );

  QString errorMessage;
  QgsVectorFileWriter::writeAsVectorFormatV3(
    layer,
    projectGpkgPath,
    layer->transformContext(),
    options,
    &errorMessage,
    nullptr,
    nullptr );

  // Check and configure layer
  QgsVectorLayer *l = new QgsVectorLayer( projectGpkgPath, layerName, "ogr" );

  Q_ASSERT( l->isValid() );

  l->setCrs( layerCrs );
  for ( int i = 0; i < l->fields().count(); ++i )
  {
    QgsField f = l->fields().at( i );
    QgsEditorWidgetSetup setup = InputUtils::getEditorWidgetSetup( f, findWidgetTypeByFieldName( f.name(), fieldsConfig ) );
    l->setEditorWidgetSetup( i, setup );
  }
  l->setRenderer( surveyLayerRenderer() );

  return l;
}

QgsVectorLayer *ProjectWizard::createTrackingLayer( const QString &trackingGpkgPath )
{
  // based on the code in https://github.com/MerginMaps/qgis-plugin/blob/master/Mergin/utils.py
  // (create_tracking_layer(), setup_tracking_layer(), set_tracking_layer_flags())

  QgsFields fields;
  fields.append( QgsField( "tracking_start_time", QVariant::DateTime ) );
  fields.append( QgsField( "tracking_end_time", QVariant::DateTime ) );
  fields.append( QgsField( "total_distance", QVariant::Double ) );
  fields.append( QgsField( "tracked_by", QVariant::String ) );

  QgsVectorFileWriter::SaveVectorOptions options;
  options.driverName = "GPKG";
  options.layerName = "tracking_layer";

  QgsVectorFileWriter *writer = QgsVectorFileWriter::create(
                                  trackingGpkgPath,
                                  fields,
                                  Qgis::WkbType::LineStringZM,
                                  QgsCoordinateReferenceSystem( "EPSG:4326" ),
                                  mSettings->transformContext(),
                                  options );
  delete writer;

  QgsVectorLayer *layer = new QgsVectorLayer( trackingGpkgPath, "tracking_layer", "ogr" );

  int idx = layer->fields().indexFromName( "fid" );
  QgsEditorWidgetSetup cfg( "Hidden", QVariantMap() );
  layer->setEditorWidgetSetup( idx, cfg );

  idx = layer->fields().indexFromName( "tracking_start_time" );
  QgsDefaultValue start_time_default;
  start_time_default.setExpression( "@tracking_start_time" );
  layer->setDefaultValueDefinition( idx, start_time_default );

  idx = layer->fields().indexFromName( "tracking_end_time" );
  QgsDefaultValue end_time_default;
  end_time_default.setExpression( "@tracking_end_time" );
  layer->setDefaultValueDefinition( idx, end_time_default );

  idx = layer->fields().indexFromName( "total_distance" );
  QgsDefaultValue distance_default;
  distance_default.setExpression( "round($length, 2)" );
  layer->setDefaultValueDefinition( idx, distance_default );

  idx = layer->fields().indexFromName( "tracked_by" );
  QgsDefaultValue user_default;
  user_default.setExpression( "@mm_username" );
  layer->setDefaultValueDefinition( idx, user_default );

  QVariantMap symbolProps;
  symbolProps["capstyle"] = "square";
  symbolProps["joinstyle"] = "bevel";
  symbolProps["line_style"] = "solid";
  symbolProps["line_width"] = "0.35";
  symbolProps["line_width_unit"] = "MM";
  symbolProps["line_color"] = QgsSymbolLayerUtils::encodeColor( QColor( "#FFA500" ) );

  layer->setRenderer( new QgsSingleSymbolRenderer( QgsLineSymbol::createSimple( symbolProps ) ) );

  layer->setReadOnly( false );
  layer->setFlags( QgsMapLayer::Identifiable | QgsMapLayer::Searchable | QgsMapLayer::Removable );

  return layer;
}

void ProjectWizard::createProject( QString const &projectNameRaw, FieldsModel *fieldsModel )
{
  if ( !CoreUtils::isValidName( projectNameRaw ) )
  {
    emit projectCreationFailed( tr( "Project name contains invalid characters" ) );
    return;
  }

  QString projectName( projectNameRaw );
  projectName = InputUtils::sanitizeNode( projectName );

  QString projectDir = CoreUtils::createUniqueProjectDirectory( mDataDir, projectName );
  QString projectFilepath( QString( "%1/%2.qgz" ).arg( projectDir ).arg( projectName ) );
  QString projectGpkgPath( QString( "%1/data.gpkg" ).arg( projectDir ) );
  QString trackingGpkgPath( QString( "%1/tracking_layer.gpkg" ).arg( projectDir ) );

  QgsProject project;

  // add layers
  QgsDataSourceUri dsUri;
  dsUri.setParam( QStringLiteral( "type" ), QStringLiteral( "xyz" ) );
  dsUri.setParam( QStringLiteral( "url" ), QStringLiteral( "%1/data/default/{z}/{x}/{y}.pbf" ).arg( TILES_URL ) );
  dsUri.setParam( QStringLiteral( "styleUrl" ), QStringLiteral( "%1/styles/default.json" ).arg( TILES_URL ) );
  dsUri.setParam( QStringLiteral( "zmin" ), QStringLiteral( "0" ) );
  dsUri.setParam( QStringLiteral( "zmax" ), QStringLiteral( "14" ) );
  QgsVectorTileLayer *bgLayer = new QgsVectorTileLayer( dsUri.encodedUri(), QStringLiteral( "OpenMapTiles (OSM)" ) );
  bool ok;
  QString error = bgLayer->loadDefaultStyle( ok );
  QgsLayerMetadata metadata;
  metadata.setRights( QStringList() << QStringLiteral( "© OpenMapTiles © OpenStreetMap contributors" ) );
  bgLayer->setMetadata( metadata );
  QgsVectorLayer *layer = createGpkgLayer( projectDir, fieldsModel->fields() );

  QgsVectorLayer *trackingLayer = createTrackingLayer( trackingGpkgPath );
  project.writeEntry( "Mergin", "PositionTracking/Enabled", true );
  project.writeEntry( "Mergin", "PositionTracking/TrackingLayer", trackingLayer->id() );
  project.writeEntry( "Mergin", "PositionTracking/UpdateFrequency", 0 );  // 0 means often (1 = normal, 2 = occasional)

  QList<QgsMapLayer *> layers;
  layers << layer << trackingLayer << bgLayer;
  project.addMapLayers( layers );

  // Configurate mapSettings
  QgsCoordinateReferenceSystem projectCrs( PROJECT_CRS_ID );
  mSettings->setExtent( bgLayer->extent() );
  mSettings->setEllipsoid( "WGS84" );
  mSettings->setDestinationCrs( projectCrs );
  mSettings->setLayers( layers );

  // Using writeProject signal to append mapCanvas project setting
  connect( &project, &QgsProject::writeProject,
           this, &ProjectWizard::writeMapCanvasSetting );

  project.setCrs( projectCrs );
  project.writePath( projectGpkgPath );
  project.write( projectFilepath );

  const QString folderName = projectDir.mid( mDataDir.size() + 1 );
  emit notifySuccess( tr( "Project %1 created" ).arg( folderName ) );
  emit projectCreated( projectDir, projectName );
}

void ProjectWizard::writeMapCanvasSetting( QDomDocument &doc )
{
  QDomNodeList nl = doc.elementsByTagName( QStringLiteral( "qgis" ) );
  if ( !nl.count() )
  {
    QgsDebugError( QStringLiteral( "Unable to find qgis element in project file" ) );
    return;
  }
  QDomNode qgisNode = nl.item( 0 );  // there should only be one, so zeroth element OK

  QDomElement mapcanvasNode = doc.createElement( QStringLiteral( "mapcanvas" ) );
  mapcanvasNode.setAttribute( QStringLiteral( "annotationsVisible" ), false );
  qgisNode.appendChild( mapcanvasNode );
  mSettings->writeXml( mapcanvasNode, doc );
}

QgsFields ProjectWizard::createFields( const QList<FieldConfiguration> fieldsConfig ) const
{

  QgsFields fields;
  for ( const FieldConfiguration &fc : fieldsConfig )
  {
    QString type = widgetToType( fc.widgetType );
    QVariant::Type qtype = parseType( type );
    QgsField field( fc.attributeName, qtype, type );
    fields.append( field );
  }
  return fields;
}

QgsSingleSymbolRenderer *ProjectWizard::surveyLayerRenderer()
{
  QgsSimpleMarkerSymbolLayer *markerLayer = new QgsSimpleMarkerSymbolLayer( Qgis::MarkerShape::Circle );
  markerLayer->setSize( 3.0 );
  markerLayer->setFillColor( QColor( "#d73027" ) );
  markerLayer->setStrokeColor( QColor( "#e8e8e8" ) );
  markerLayer->setStrokeWidth( 0.4 );
  QgsMarkerSymbol *symbol = new QgsMarkerSymbol( QgsSymbolLayerList() << markerLayer );
  return new QgsSingleSymbolRenderer( symbol );
}

QVariant::Type ProjectWizard::parseType( const QString &type ) const
{
  if ( type == QLatin1String( "text" ) )
    return QVariant::String;
  else if ( type == QLatin1String( "integer" ) )
    return QVariant::Int;
  else if ( type == QLatin1String( "integer64" ) )
    return QVariant::Int;
  else if ( type == QLatin1String( "real" ) )
    return QVariant::Double;
  else if ( type == QLatin1String( "date" ) )
    return QVariant::Date;
  else if ( type == QLatin1String( "datetime" ) )
    return QVariant::DateTime;
  else if ( type == QLatin1String( "bool" ) )
    return QVariant::Bool;
  else if ( type == QLatin1String( "binary" ) )
    return QVariant::ByteArray;

  return QVariant::Invalid;
}

QString ProjectWizard::widgetToType( const QString &widgetType ) const
{
  if ( widgetType == QStringLiteral( "TextEdit" ) )
    return QStringLiteral( "text" );
  else if ( widgetType == QStringLiteral( "Range" ) )
    return QStringLiteral( "integer" );
  else if ( widgetType == QStringLiteral( "DateTime" ) )
    return QStringLiteral( "datetime" );
  else if ( widgetType == QStringLiteral( "CheckBox" ) )
    return QStringLiteral( "bool" );
  else if ( widgetType == QStringLiteral( "ExternalResource" ) )
    return QStringLiteral( "text" );

  return QStringLiteral( "text" );
}

QString ProjectWizard::findWidgetTypeByFieldName( const QString name, const QList<FieldConfiguration> fieldsConfig ) const
{

  for ( int i = 0; i < fieldsConfig.count(); ++i )
  {
    if ( fieldsConfig.at( i ).attributeName == name )
      return fieldsConfig.at( i ).widgetType;
  }
  return QString( "TextEdit" );
}
