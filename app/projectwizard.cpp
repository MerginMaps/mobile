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
}

QgsVectorLayer *ProjectWizard::createGpkgLayer( QString const &projectDir )
{
  QString gpkgName( QStringLiteral( "data" ) );
  QString projectGpkgPath( QString( "%1/%2.%3" ).arg( projectDir ).arg( gpkgName ).arg( "gpkg" ) );
  QString layerName( QStringLiteral( "Points" ) );
  QgsCoordinateReferenceSystem layerCrs( " EPSG:4326" );
  QgsFields predefinedFields = mFieldsModel->fields();

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
  QgsVectorLayer *layer2 = new QgsVectorLayer( projectGpkgPath, "Points", "ogr" );

  Q_ASSERT( layer2->isValid() );

  layer2->startEditing();
  layer2->setCrs( layerCrs );
  for ( int i = 0; i < layer2->fields().count(); ++i )
  {
    QgsField f = layer2->fields().at( i );
    QgsEditorWidgetSetup setup = getEditorWidget( f, mFieldsModel->findWidgetTypeByFieldName( f.name() ) );
    layer2->setEditorWidgetSetup( i, setup );
  }
  layer2->commitChanges();

  return layer2;
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

  emit projectCreated( projectDir, projectName );
}

QgsEditorWidgetSetup ProjectWizard::getEditorWidget( const QgsField &field, const QString &widgetType )
{

  // TODO support Slider, SpinBox
  // TODO DateTime formatter: qgsdatetimefieldformatter
  qDebug() << "FIELD SETUP" << field.name() << field.typeName() << field.type() << widgetType;
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
    // TODO better widget for Range: Slider, etc
    else if ( widgetType == QStringLiteral( "Range" ) )
    {
      config.insert( QStringLiteral( "Style" ), QStringLiteral( "SpinBox" ) );
    }
    else if ( widgetType == QStringLiteral( "ExternalResource" ) )
    {
      config.insert( QStringLiteral( "RelativeStorage" ), 1 );
      config.insert( QStringLiteral( "StorageMode" ), 0 );
    }

    return QgsEditorWidgetSetup( widgetType, config );;
  }
}
