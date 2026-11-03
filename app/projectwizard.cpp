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
#include "qgsmarkersymbollayer.h"
#include "qgslinesymbol.h"
#include "qgssymbollayer.h"
#include "qgssymbollayerutils.h"
#include "qgssymbol.h"
#include "qgsmarkersymbol.h"
#include "qgssinglesymbolrenderer.h"
#include "qgscategorizedsymbolrenderer.h"
#include "qgsfillsymbol.h"
#include "qgsproperty.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

const QString TILES_URL = QStringLiteral( "https://tiles.merginmaps.com" );

ProjectWizard::ProjectWizard( const QString &dataDir, QObject *parent )
  : QObject( parent )
  , mDataDir( dataDir )
{

  mSettings = std::unique_ptr<QgsMapSettings>( new QgsMapSettings );
}

QgsVectorLayer *ProjectWizard::createGpkgLayer( QString const &projectDir, QList<FieldConfiguration> const &fieldsConfig )
{
  const QString gpkgName( QStringLiteral( "data" ) );
  const QString projectGpkgPath( QString( "%1/%2.%3" ).arg( projectDir ).arg( gpkgName ).arg( "gpkg" ) );
  const QString layerName( QStringLiteral( "Survey" ) );
  const QgsCoordinateReferenceSystem layerCrs( LAYER_CRS_ID );
  QgsFields predefinedFields = createFields( fieldsConfig );

  // Write layer as gpkg
  QgsVectorLayer *layer = new QgsVectorLayer( QStringLiteral( "PointZ?crs=%1" ).arg( LAYER_CRS_ID ), layerName, "memory" );
  layer->startEditing();
  layer->setCrs( layerCrs );
  for ( const QgsField &f : predefinedFields )
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
    const QgsEditorWidgetSetup setup = editorWidgetSetup( f, findWidgetTypeByFieldName( f.name(), fieldsConfig ) );
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

  const QgsVectorFileWriter *writer = QgsVectorFileWriter::create(
                                        trackingGpkgPath,
                                        fields,
                                        Qgis::WkbType::LineStringZM,
                                        QgsCoordinateReferenceSystem( "EPSG:4326" ),
                                        mSettings->transformContext(),
                                        options );
  delete writer;

  QgsVectorLayer *layer = new QgsVectorLayer( trackingGpkgPath, "tracking_layer", "ogr" );

  int idx = layer->fields().indexFromName( "fid" );
  const QgsEditorWidgetSetup cfg( "Hidden", QVariantMap() );
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

  layer->setRenderer( new QgsSingleSymbolRenderer( QgsLineSymbol::createSimple( symbolProps ).release() ) );

  layer->setReadOnly( false );
  layer->setFlags( QgsMapLayer::Identifiable | QgsMapLayer::Searchable | QgsMapLayer::Removable );

  return layer;
}

void ProjectWizard::createProject( QString const &projectNameRaw, const FieldsModel *fieldsModel )
{
  if ( !CoreUtils::isValidName( projectNameRaw ) )
  {
    emit projectCreationFailed( tr( "Project name contains invalid characters" ) );
    return;
  }

  QString projectName( projectNameRaw );
  projectName = InputUtils::sanitizeNode( projectName );

  const QString projectDir = CoreUtils::createUniqueProjectDirectory( mDataDir, projectName );
  const QString projectFilepath( QString( "%1/%2.qgz" ).arg( projectDir ).arg( projectName ) );
  const QString projectGpkgPath( QString( "%1/data.gpkg" ).arg( projectDir ) );
  const QString trackingGpkgPath( QString( "%1/tracking_layer.gpkg" ).arg( projectDir ) );

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
  const QgsCoordinateReferenceSystem projectCrs( PROJECT_CRS_ID );
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

namespace
{
  struct ObservationType
  {
    const char *value;
    const char *label;
    const char *color;
    Qgis::MarkerShape shape;
    bool form;
  };

  // common observations for ad-hoc field collection; "note" opens the form to type the text
  const QList<ObservationType> OBSERVATION_TYPES =
  {
    { "note", QT_TRANSLATE_NOOP( "ProjectWizard", "Note" ), "#3E5C76", Qgis::MarkerShape::Square, true },
    { "inaccessible_road", QT_TRANSLATE_NOOP( "ProjectWizard", "Inaccessible road" ), "#D7191C", Qgis::MarkerShape::Circle, false },
    { "flooded_road", QT_TRANSLATE_NOOP( "ProjectWizard", "Flooded road" ), "#0098C7", Qgis::MarkerShape::Circle, false },
    { "checkpoint", QT_TRANSLATE_NOOP( "ProjectWizard", "Checkpoint" ), "#F28E2B", Qgis::MarkerShape::Diamond, false },
    { "damaged_bridge", QT_TRANSLATE_NOOP( "ProjectWizard", "Damaged bridge" ), "#7B3294", Qgis::MarkerShape::Diamond, false },
    { "security_incident", QT_TRANSLATE_NOOP( "ProjectWizard", "Security incident" ), "#1A1A1A", Qgis::MarkerShape::Triangle, false },
    { "health_facility", QT_TRANSLATE_NOOP( "ProjectWizard", "Health facility" ), "#2CA25F", Qgis::MarkerShape::Square, false },
    { "health_warning", QT_TRANSLATE_NOOP( "ProjectWizard", "Health warning" ), "#A6611A", Qgis::MarkerShape::Triangle, false },
    { "water_point", QT_TRANSLATE_NOOP( "ProjectWizard", "Water point" ), "#1F5FBF", Qgis::MarkerShape::Circle, false },
    { "displaced_people", QT_TRANSLATE_NOOP( "ProjectWizard", "Displaced people" ), "#66A61E", Qgis::MarkerShape::Pentagon, false },
  };

  // media captured with quick capture are drawn by the attachment they carry
  const QList<ObservationType> MEDIA_SYMBOLS =
  {
    { "photo_note", QT_TRANSLATE_NOOP( "ProjectWizard", "Photo" ), "#E7298A", Qgis::MarkerShape::Circle, false },
    { "audio_note", QT_TRANSLATE_NOOP( "ProjectWizard", "Audio note" ), "#00897B", Qgis::MarkerShape::Circle, false },
    { "video_note", QT_TRANSLATE_NOOP( "ProjectWizard", "Video" ), "#3949AB", Qgis::MarkerShape::Circle, false },
    { "field_note", QT_TRANSLATE_NOOP( "ProjectWizard", "Field note" ), "#6B6B6B", Qgis::MarkerShape::Circle, false },
  };

  QgsMarkerSymbol *observationSymbol( const ObservationType &type )
  {
    QgsSimpleMarkerSymbolLayer *marker = new QgsSimpleMarkerSymbolLayer( type.shape, 4.0 );
    marker->setFillColor( QColor( type.color ) );
    marker->setStrokeColor( QColor( QStringLiteral( "white" ) ) );
    marker->setStrokeWidth( 0.6 );
    return new QgsMarkerSymbol( QgsSymbolLayerList() << marker );
  }
}

bool ProjectWizard::prepareProjectDir( const QString &projectNameRaw, QString &projectDir, QString &projectName )
{
  if ( !CoreUtils::isValidName( projectNameRaw ) )
  {
    emit projectCreationFailed( tr( "Project name contains invalid characters" ) );
    return false;
  }

  projectName = InputUtils::sanitizeNode( projectNameRaw );
  projectDir = CoreUtils::createUniqueProjectDirectory( mDataDir, projectName );
  return true;
}

QgsVectorTileLayer *ProjectWizard::createBackgroundLayer()
{
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
  return bgLayer;
}

void ProjectWizard::writeProject( QgsProject &project, const QString &projectDir, const QString &projectName, QgsMapLayer *extentLayer, const QList<QgsMapLayer *> &layers )
{
  // plain .qgs: no archive step, so nothing can fail while zipping the project
  const QString projectFilepath( QString( "%1/%2.qgs" ).arg( projectDir ).arg( projectName ) );

  project.addMapLayers( layers );

  const QgsCoordinateReferenceSystem projectCrs( PROJECT_CRS_ID );
  mSettings->setExtent( extentLayer->extent() );
  mSettings->setEllipsoid( "WGS84" );
  mSettings->setDestinationCrs( projectCrs );
  mSettings->setLayers( layers );

  // Using writeProject signal to append mapCanvas project setting
  connect( &project, &QgsProject::writeProject,
           this, &ProjectWizard::writeMapCanvasSetting );

  project.setCrs( projectCrs );
  if ( !project.write( projectFilepath ) )
  {
    CoreUtils::log( QStringLiteral( "Create project" ), QStringLiteral( "Failed to write %1: %2" ).arg( projectFilepath, project.error() ) );
    emit projectCreationFailed( tr( "Failed to create project" ) );
    return;
  }

  const QString folderName = projectDir.mid( mDataDir.size() + 1 );
  emit notifySuccess( tr( "Project %1 created" ).arg( folderName ) );
  emit projectCreated( projectDir, projectName );
}

QgsVectorLayer *ProjectWizard::createObservationsLayer( const QString &projectDir )
{
  const QString gpkgPath( QStringLiteral( "%1/data.gpkg" ).arg( projectDir ) );
  const QString layerName( tr( "Observations" ) );

  QgsFields fields;
  fields.append( QgsField( QStringLiteral( "obs_type" ), QMetaType::QString, QStringLiteral( "text" ) ) );
  fields.append( QgsField( QStringLiteral( "notes" ), QMetaType::QString, QStringLiteral( "text" ) ) );
  fields.append( QgsField( QStringLiteral( "photo" ), QMetaType::QString, QStringLiteral( "text" ) ) );
  fields.append( QgsField( QStringLiteral( "audio" ), QMetaType::QString, QStringLiteral( "text" ) ) );
  fields.append( QgsField( QStringLiteral( "video" ), QMetaType::QString, QStringLiteral( "text" ) ) );
  fields.append( QgsField( QStringLiteral( "surveyor" ), QMetaType::QString, QStringLiteral( "text" ) ) );
  fields.append( QgsField( QStringLiteral( "captured_at" ), QMetaType::QDateTime, QStringLiteral( "datetime" ) ) );
  fields.append( QgsField( QStringLiteral( "gps_accuracy" ), QMetaType::Double, QStringLiteral( "real" ) ) );

  QgsVectorFileWriter::SaveVectorOptions options;
  options.driverName = QStringLiteral( "GPKG" );
  options.layerName = layerName;
  options.fileEncoding = QStringLiteral( "UTF-8" );
  std::unique_ptr<QgsVectorFileWriter> writer( QgsVectorFileWriter::create(
        gpkgPath, fields, Qgis::WkbType::PointZ, QgsCoordinateReferenceSystem( LAYER_CRS_ID ),
        QgsCoordinateTransformContext(), options ) );
  writer.reset();

  QgsVectorLayer *layer = new QgsVectorLayer( QStringLiteral( "%1|layername=%2" ).arg( gpkgPath, layerName ), layerName, QStringLiteral( "ogr" ) );

  QVariantList valueMap;
  for ( const ObservationType &type : OBSERVATION_TYPES )
    valueMap << QVariantMap( { { tr( type.label ), QString::fromLatin1( type.value ) } } );
  valueMap << QVariantMap( { { tr( "Field note (media)" ), QStringLiteral( "field_note" ) } } );

  const QgsFields layerFields = layer->fields();
  for ( int i = 0; i < layerFields.count(); ++i )
  {
    const QgsField field = layerFields.at( i );
    const QString name = field.name();
    if ( name == QLatin1String( "obs_type" ) )
      layer->setEditorWidgetSetup( i, QgsEditorWidgetSetup( QStringLiteral( "ValueMap" ), { { QStringLiteral( "map" ), valueMap } } ) );
    else if ( name == QLatin1String( "notes" ) )
      layer->setEditorWidgetSetup( i, InputUtils::getEditorWidgetSetup( field, QStringLiteral( "TextEdit" ), { { QStringLiteral( "isMultiline" ), true } } ) );
    else if ( name == QLatin1String( "photo" ) )
      layer->setEditorWidgetSetup( i, editorWidgetSetup( field, QStringLiteral( "ExternalResource" ) ) );
    else if ( name == QLatin1String( "audio" ) )
      layer->setEditorWidgetSetup( i, editorWidgetSetup( field, QStringLiteral( "ExternalResourceAudio" ) ) );
    else if ( name == QLatin1String( "video" ) )
      layer->setEditorWidgetSetup( i, editorWidgetSetup( field, QStringLiteral( "ExternalResourceVideo" ) ) );
    else
      layer->setEditorWidgetSetup( i, InputUtils::getEditorWidgetSetup( field ) );
  }

  layer->setDefaultValueDefinition( layerFields.indexOf( QStringLiteral( "surveyor" ) ), QgsDefaultValue( QStringLiteral( "@mm_username" ) ) );
  layer->setDefaultValueDefinition( layerFields.indexOf( QStringLiteral( "captured_at" ) ), QgsDefaultValue( QStringLiteral( "now()" ) ) );
  layer->setDefaultValueDefinition( layerFields.indexOf( QStringLiteral( "gps_accuracy" ) ), QgsDefaultValue( QStringLiteral( "round(@position_horizontal_accuracy, 1)" ) ) );

  layer->setFieldAlias( layerFields.indexOf( QStringLiteral( "obs_type" ) ), tr( "Observation" ) );
  layer->setFieldAlias( layerFields.indexOf( QStringLiteral( "notes" ) ), tr( "Notes" ) );
  layer->setFieldAlias( layerFields.indexOf( QStringLiteral( "photo" ) ), tr( "Photo" ) );
  layer->setFieldAlias( layerFields.indexOf( QStringLiteral( "audio" ) ), tr( "Audio note" ) );
  layer->setFieldAlias( layerFields.indexOf( QStringLiteral( "video" ) ), tr( "Video" ) );
  layer->setFieldAlias( layerFields.indexOf( QStringLiteral( "surveyor" ) ), tr( "Surveyor" ) );
  layer->setFieldAlias( layerFields.indexOf( QStringLiteral( "captured_at" ) ), tr( "Captured at" ) );
  layer->setFieldAlias( layerFields.indexOf( QStringLiteral( "gps_accuracy" ) ), tr( "GPS accuracy (m)" ) );

  // one colour and shape per type; media points by the attachment they carry
  const QString classification = QStringLiteral(
                                   "CASE WHEN \"obs_type\" = 'field_note' THEN "
                                   "CASE WHEN coalesce(\"video\", '') <> '' THEN 'video_note' "
                                   "WHEN coalesce(\"audio\", '') <> '' THEN 'audio_note' "
                                   "WHEN coalesce(\"photo\", '') <> '' THEN 'photo_note' "
                                   "ELSE 'field_note' END ELSE \"obs_type\" END" );
  QgsCategoryList categories;
  for ( const ObservationType &type : OBSERVATION_TYPES + MEDIA_SYMBOLS )
    categories << QgsRendererCategory( QString::fromLatin1( type.value ), observationSymbol( type ), tr( type.label ) );
  layer->setRenderer( new QgsCategorizedSymbolRenderer( classification, categories ) );

  return layer;
}

QgsVectorLayer *ProjectWizard::createMapSketchesLayer( const QString &projectDir )
{
  // same layer as the Mergin Maps QGIS plugin creates for map sketching
  const QString gpkgPath( QStringLiteral( "%1/map_sketches.gpkg" ).arg( projectDir ) );
  const QString layerName( QStringLiteral( "Map sketches" ) );

  QgsFields fields;
  fields.append( QgsField( QStringLiteral( "color" ), QMetaType::QString ) );
  fields.append( QgsField( QStringLiteral( "author" ), QMetaType::QString ) );
  fields.append( QgsField( QStringLiteral( "created_at" ), QMetaType::QDateTime ) );
  fields.append( QgsField( QStringLiteral( "width" ), QMetaType::Double ) );
  fields.append( QgsField( QStringLiteral( "attr1" ), QMetaType::Double ) );
  fields.append( QgsField( QStringLiteral( "attr2" ), QMetaType::Double ) );
  fields.append( QgsField( QStringLiteral( "attr3" ), QMetaType::QString ) );
  fields.append( QgsField( QStringLiteral( "attr4" ), QMetaType::QString ) );

  QgsVectorFileWriter::SaveVectorOptions options;
  options.driverName = QStringLiteral( "GPKG" );
  options.layerName = layerName;
  std::unique_ptr<QgsVectorFileWriter> writer( QgsVectorFileWriter::create(
        gpkgPath, fields, Qgis::WkbType::MultiLineStringZM, QgsCoordinateReferenceSystem( LAYER_CRS_ID ),
        QgsCoordinateTransformContext(), options ) );
  writer.reset();

  QgsVectorLayer *layer = new QgsVectorLayer( gpkgPath, layerName, QStringLiteral( "ogr" ) );
  const QgsFields layerFields = layer->fields();
  layer->setEditorWidgetSetup( layerFields.indexOf( QStringLiteral( "fid" ) ), QgsEditorWidgetSetup( QStringLiteral( "Hidden" ), QVariantMap() ) );
  layer->setDefaultValueDefinition( layerFields.indexOf( QStringLiteral( "author" ) ), QgsDefaultValue( QStringLiteral( "@mm_username" ) ) );
  layer->setDefaultValueDefinition( layerFields.indexOf( QStringLiteral( "created_at" ) ), QgsDefaultValue( QStringLiteral( "now()" ) ) );
  layer->setDefaultValueDefinition( layerFields.indexOf( QStringLiteral( "width" ) ), QgsDefaultValue( QStringLiteral( "0.6" ) ) );

  QVariantMap props;
  props[QStringLiteral( "line_width" )] = QStringLiteral( "0.6" );
  props[QStringLiteral( "line_color" )] = QgsSymbolLayerUtils::encodeColor( QColor( QStringLiteral( "#FFFFFF" ) ) );
  std::unique_ptr<QgsLineSymbol> symbol = QgsLineSymbol::createSimple( props );
  symbol->symbolLayer( 0 )->setDataDefinedProperty( QgsSymbolLayer::Property::StrokeColor, QgsProperty::fromExpression( QStringLiteral( "\"color\"" ) ) );
  symbol->symbolLayer( 0 )->setDataDefinedProperty( QgsSymbolLayer::Property::StrokeWidth, QgsProperty::fromExpression( QStringLiteral( "\"width\"" ) ) );
  layer->setRenderer( new QgsSingleSymbolRenderer( symbol.release() ) );
  return layer;
}

void ProjectWizard::createFieldObservationProject( QString const &projectNameRaw )
{
  QString projectDir;
  QString projectName;
  if ( !prepareProjectDir( projectNameRaw, projectDir, projectName ) )
    return;

  // no embedded project style database: it is a project attachment, and saving attachments needs a zip archive,
  // which fails on some Android devices (the project file is then not written at all)
  QgsProject project( nullptr, Qgis::ProjectCapabilities() );

  QgsVectorTileLayer *bgLayer = createBackgroundLayer();
  QgsVectorLayer *observations = createObservationsLayer( projectDir );
  QgsVectorLayer *sketches = createMapSketchesLayer( projectDir );
  QgsVectorLayer *trackingLayer = createTrackingLayer( QStringLiteral( "%1/tracking_layer.gpkg" ).arg( projectDir ) );

  project.writeEntry( "Mergin", "PositionTracking/Enabled", true );
  project.writeEntry( "Mergin", "PositionTracking/TrackingLayer", trackingLayer->id() );
  project.writeEntry( "Mergin", "PositionTracking/UpdateFrequency", 0 );
  project.writeEntry( "Mergin", "MapSketching/Enabled", true );
  project.writeEntry( "Mergin", "MapSketching/Layer", sketches->id() );
  project.writeEntry( "Mergin", "PhotoQuality", 2 );

  // quick capture for the observations, see FieldProfile and QuickCaptureController
  QJsonArray types;
  for ( const ObservationType &type : OBSERVATION_TYPES )
  {
    QJsonObject t { { QStringLiteral( "label" ), tr( type.label ) }, { QStringLiteral( "value" ), QString::fromLatin1( type.value ) } };
    if ( type.form )
      t[QStringLiteral( "form" )] = true;
    types << t;
  }
  const QJsonObject profile
  {
    { QStringLiteral( "version" ), 1 },
    {
      QStringLiteral( "quickCapture" ), QJsonObject
      {
        { QStringLiteral( "layer" ), observations->name() },
        { QStringLiteral( "typeField" ), QStringLiteral( "obs_type" ) },
        { QStringLiteral( "types" ), types },
        {
          QStringLiteral( "media" ), QJsonObject
          {
            { QStringLiteral( "typeValue" ), QStringLiteral( "field_note" ) },
            { QStringLiteral( "photoField" ), QStringLiteral( "photo" ) },
            { QStringLiteral( "audioField" ), QStringLiteral( "audio" ) },
            { QStringLiteral( "videoField" ), QStringLiteral( "video" ) }
          }
        }
      }
    }
  };
  QFile profileFile( QStringLiteral( "%1/field-profile.json" ).arg( projectDir ) );
  if ( profileFile.open( QIODevice::WriteOnly ) )
    profileFile.write( QJsonDocument( profile ).toJson() );

  writeProject( project, projectDir, projectName, bgLayer, QList<QgsMapLayer *>() << observations << sketches << trackingLayer << bgLayer );
}

void ProjectWizard::writeMapCanvasSetting( QDomDocument &doc )
{
  const QDomNodeList nl = doc.elementsByTagName( QStringLiteral( "qgis" ) );
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

QgsFields ProjectWizard::createFields( const QList<FieldConfiguration> &fieldsConfig ) const
{

  QgsFields fields;
  for ( const FieldConfiguration &fc : fieldsConfig )
  {
    QString type = widgetToType( fc.widgetType );
    const QMetaType::Type qtype = parseType( type );
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

QMetaType::Type ProjectWizard::parseType( const QString &type ) const
{
  if ( type == QLatin1String( "text" ) )
    return QMetaType::QString;

  if ( type == QLatin1String( "integer" ) || type == QLatin1String( "integer64" ) )
    return QMetaType::Int;

  if ( type == QLatin1String( "real" ) )
    return QMetaType::Double;

  if ( type == QLatin1String( "date" ) )
    return QMetaType::QDate;

  if ( type == QLatin1String( "datetime" ) )
    return QMetaType::QDateTime;

  if ( type == QLatin1String( "bool" ) )
    return QMetaType::Bool;

  if ( type == QLatin1String( "binary" ) )
    return QMetaType::QByteArray;

  return QMetaType::UnknownType;
}

QString ProjectWizard::widgetToType( const QString &widgetType ) const
{
  if ( widgetType == QStringLiteral( "TextEdit" ) )
    return QStringLiteral( "text" );

  if ( widgetType == QStringLiteral( "Range" ) )
    return QStringLiteral( "integer" );

  if ( widgetType == QStringLiteral( "DateTime" ) )
    return QStringLiteral( "datetime" );

  if ( widgetType == QStringLiteral( "CheckBox" ) )
    return QStringLiteral( "bool" );

  if ( widgetType == QStringLiteral( "ExternalResource" ) ||
       widgetType == QStringLiteral( "ExternalResourceAudio" ) ||
       widgetType == QStringLiteral( "ExternalResourceVideo" ) )
    return QStringLiteral( "text" );

  return QStringLiteral( "text" );
}

QgsEditorWidgetSetup ProjectWizard::editorWidgetSetup( const QgsField &field, const QString &widgetType ) const
{
  // audio and video are attachment (ExternalResource) widgets with the document viewer set accordingly
  if ( widgetType == QStringLiteral( "ExternalResourceAudio" ) )
  {
    return InputUtils::getEditorWidgetSetup( field, QStringLiteral( "ExternalResource" ), { { QStringLiteral( "DocumentViewer" ), static_cast<int>( InputUtils::DocumentViewerAudio ) } } );
  }

  if ( widgetType == QStringLiteral( "ExternalResourceVideo" ) )
  {
    return InputUtils::getEditorWidgetSetup( field, QStringLiteral( "ExternalResource" ), { { QStringLiteral( "DocumentViewer" ), static_cast<int>( InputUtils::DocumentViewerVideo ) } } );
  }

  return InputUtils::getEditorWidgetSetup( field, widgetType );
}

QString ProjectWizard::findWidgetTypeByFieldName( const QString &name, const QList<FieldConfiguration> &fieldsConfig ) const
{

  for ( int i = 0; i < fieldsConfig.count(); ++i )
  {
    if ( fieldsConfig.at( i ).attributeName == name )
      return fieldsConfig.at( i ).widgetType;
  }
  return QString( "TextEdit" );
}
