#include "projectwizard.h"
#include "inpututils.h"

#include "qgsproject.h"
#include "qgsrasterlayer.h"
#include "qgsvectorlayer.h"
#include "qgsvectorfilewriter.h"
#include "qgsdatetimefieldformatter.h"

#include <ogr_api.h>
#include <ogr_srs_api.h>
#include <gdal_version.h>

#include <cpl_error.h>
#include <cpl_string.h>

ProjectWizard::ProjectWizard( const QString &dataDir, FieldsModel *fieldsModel, QObject *parent )
  : QObject( parent )
  , mDataDir( dataDir )
  , mFieldsModel( fieldsModel )
{
  QObject::connect( mFieldsModel, &FieldsModel::notify, this, &ProjectWizard::notify );
}

QgsVectorLayer *ProjectWizard::createGpkgLayer( QString const &projectDir )
{
  QString gpkgName( QStringLiteral( "data" ) );
  QString projectGpkgPath( QString( "%1/%2.%3" ).arg( projectDir ).arg( gpkgName ).arg( "gpkg" ) );
  QString layerName( QStringLiteral( "Points" ) );
  QgsCoordinateReferenceSystem layerCrs( "EPSG:4326" );
  QgsFields predefinedFields = createFields( mFieldsModel->fields() );

  // Write layer as gpkg
  QgsVectorLayer *layer = new QgsVectorLayer( QStringLiteral( "Point?crs=EPSG:4326" ), layerName, "memory" );
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
  options.fileEncoding = QStringLiteral( "UTF-8" ); // TODO check name/col name with special chars
  std::unique_ptr< QgsVectorFileWriter > writer( QgsVectorFileWriter::create( projectGpkgPath, predefinedFields, QgsWkbTypes::Point, layerCrs, QgsCoordinateTransformContext(), options ) );


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
  QgsVectorLayer *l = new QgsVectorLayer( projectGpkgPath, "Points", "ogr" );

  Q_ASSERT( l->isValid() );

  l->startEditing();
  l->setCrs( layerCrs );
  for ( int i = 0; i < l->fields().count(); ++i )
  {
    QgsField f = l->fields().at( i );
    QgsEditorWidgetSetup setup = getEditorWidget( f, mFieldsModel->findWidgetTypeByFieldName( f.name() ) );
    l->setEditorWidgetSetup( i, setup );
  }
  l->commitChanges();

  return l;
}

void ProjectWizard::createProject( QString const &projectName )
{
  QString projectDir = InputUtils::createUniqueProjectDirectory( mDataDir, projectName );
  QString projectFilepath( QString( "%1/%2.%3" ).arg( projectDir ).arg( projectName ).arg( "qgs" ) );
  QString gpkgName( QStringLiteral( "data" ) );
  QString projectGpkgPath( QString( "%1/%2.%3" ).arg( projectDir ).arg( gpkgName ).arg( "gpkg" ) );

  QgsProject project;

  // add background layer
  QString urlWithParams( "tilePixelRatio=1&type=xyz&url=https://tile.openstreetmap.org/%7Bz%7D/%7Bx%7D/%7By%7D.png&zmax=19&zmin=0" );
  QgsRasterLayer *bgLayer = new QgsRasterLayer( urlWithParams, "OpenStreetMap", "wms" );
  project.addMapLayer( bgLayer );

  // Add vector layer
  QgsVectorLayer *layer = createGpkgLayer( projectDir );
  project.addMapLayer( layer );

  project.setCrs( QgsCoordinateReferenceSystem( QStringLiteral( "EPSG:3857" ) ) );
  project.writePath( projectGpkgPath );
  project.write( projectFilepath );

  emit notify( tr( "Project %1 created" ).arg( projectName ) );
  emit projectCreated( projectDir, projectName );
}

QgsEditorWidgetSetup ProjectWizard::getEditorWidget( const QgsField &field, const QString &widgetType )
{
  if ( field.name() == QStringLiteral( "fid" ) )
    return QgsEditorWidgetSetup( QStringLiteral( "Hidden" ), QVariantMap() );

  if ( widgetType.isEmpty() )
  {
    return QgsEditorWidgetSetup( QStringLiteral( "TextEdit" ), QVariantMap() );
  }
  else
  {
    QVariantMap config;
    if ( widgetType == QStringLiteral( "TextEdit" ) )
    {
      config.insert( QStringLiteral( "isMultiline" ), false );
      config.insert( QStringLiteral( "UseHtml" ), false );
    }
    else if ( widgetType == QStringLiteral( "DateTime" ) )
    {
      config.insert( QStringLiteral( "field_format" ), QgsDateTimeFieldFormatter::DATETIME_FORMAT );
      config.insert( QStringLiteral( "display_format" ), QgsDateTimeFieldFormatter::DATETIME_FORMAT );
    }
    else if ( widgetType == QStringLiteral( "Range" ) )
    {
      config.insert( QStringLiteral( "Style" ), QStringLiteral( "SpinBox" ) );
      config.insert( QStringLiteral( "Precision" ), QStringLiteral( "0" ) );
      config.insert( QStringLiteral( "Min" ), QString::number( INT_MIN ) );
      config.insert( QStringLiteral( "Max" ), QString::number( INT_MAX ) );
      config.insert( QStringLiteral( "Step" ), 1 );
    }
    else if ( widgetType == QStringLiteral( "ExternalResource" ) )
    {
      config.insert( QStringLiteral( "RelativeStorage" ), QStringLiteral( "1" ) );
      config.insert( QStringLiteral( "StorageMode" ), QStringLiteral( "0" ) );
      config.insert( QStringLiteral( "PropertyCollection" ), QVariantMap() );
      QgsPropertyCollection collection;
      config.insert( QStringLiteral( "PropertyCollection" ), collection.toVariant( QgsPropertiesDefinition() ) );
    }

    return QgsEditorWidgetSetup( widgetType, config );
  }
}

QgsFields ProjectWizard::createFields( const QList<FieldConfiguration> fieldsConfig ) const
{

  QgsFields fields;
  for ( const FieldConfiguration fc : fieldsConfig )
  {
    QString type = widgetToType( fc.widgetType );
    QVariant::Type qtype = parseType( type );
    QgsField field( fc.attributeName, qtype, type );
    fields.append( field );
  }
  return fields;
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
