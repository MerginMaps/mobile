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
#include "qgsrasterlayer.h"
#include "qgsvectorlayer.h"
#include "qgsvectorfilewriter.h"
#include "qgsdatetimefieldformatter.h"
#include "qgsmarkersymbollayer.h"
#include "qgssymbollayer.h"
#include "qgssymbol.h"
#include "qgsmarkersymbol.h"
#include "qgssinglesymbolrenderer.h"
#include "inpututils.h"

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
  std::unique_ptr< QgsVectorFileWriter > writer( QgsVectorFileWriter::create( projectGpkgPath, predefinedFields, QgsWkbTypes::PointZ, layerCrs, QgsCoordinateTransformContext(), options ) );


  QString errorMessage;
  QgsVectorFileWriter::writeAsVectorFormatV2(
    layer,
    projectGpkgPath,
    layer->transformContext(),
    options,
    nullptr,
    nullptr,
    &errorMessage );

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

void ProjectWizard::createProject( QString const &projectName, FieldsModel *fieldsModel )
{
  QString projectDir = CoreUtils::createUniqueProjectDirectory( mDataDir, projectName );
  QString projectFilepath( QString( "%1/%2.%3" ).arg( projectDir ).arg( projectName ).arg( "qgs" ) );
  QString gpkgName( QStringLiteral( "data" ) );
  QString projectGpkgPath( QString( "%1/%2.%3" ).arg( projectDir ).arg( gpkgName ).arg( "gpkg" ) );

  QgsProject project;

  // add layers
  QString urlWithParams( BG_MAPS_CONFIG );
  QgsRasterLayer *bgLayer = new QgsRasterLayer( BG_MAPS_CONFIG, QStringLiteral( "OpenStreetMap" ), QStringLiteral( "wms" ) );
  QgsVectorLayer *layer = createGpkgLayer( projectDir, fieldsModel->fields() );
  QList<QgsMapLayer *> layers;
  layers << layer << bgLayer;
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

  emit notify( tr( "Project %1 created" ).arg( projectName ) );
  emit projectCreated( projectDir, projectName );
}

void ProjectWizard::writeMapCanvasSetting( QDomDocument &doc )
{
  QDomNodeList nl = doc.elementsByTagName( QStringLiteral( "qgis" ) );
  if ( !nl.count() )
  {
    QgsDebugMsg( QStringLiteral( "Unable to find qgis element in project file" ) );
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
  QgsSimpleMarkerSymbolLayer *markerLayer = new QgsSimpleMarkerSymbolLayer( QgsSimpleMarkerSymbolLayer::Circle );
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
