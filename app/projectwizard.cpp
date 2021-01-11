#include "projectwizard.h"
#include "inpututils.h"

#include "qgsproject.h"
#include "qgsvectorlayer.h"
#include "qgsvectorfilewriter.h"

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

  QgsFields fields = mFieldsModel->fields();
  //QgsFields fields;
  QgsVectorLayer *layer = new QgsVectorLayer( QStringLiteral( "Point?crs=EPSG:4326" ), layerName, "memory" );
  //layer->setCrs(layerCrs);
  qDebug() << "!!! SAVING Layer" << layer->isValid() <<  projectGpkgPath;

  //QgsVectorDataProvider *dataProvider = layer->dataProvider();
  layer->startEditing();
  for ( QgsField f : fields )
  {
    //dataProvider->addAttributes()
    layer->addAttribute( f );
  }
  layer->commitChanges();

  QgsVectorFileWriter::SaveVectorOptions options;
//  options.driverName = QStringLiteral( "GPKG" );
//  options.layerName = layerName;
//  options.fileEncoding = QStringLiteral( "UTF-8" );
  std::unique_ptr< QgsVectorFileWriter > writer( QgsVectorFileWriter::create( projectGpkgPath, fields, QgsWkbTypes::Point, layerCrs, QgsCoordinateTransformContext(), options ) );


  QString errorMessage;
  QgsVectorFileWriter::writeAsVectorFormatV2(
    layer,
    projectGpkgPath,
    layer->transformContext(),
    options,
    nullptr,
    nullptr,
    &errorMessage );

//  QgsVectorLayer vl2( QStringLiteral( "%1|layername=%2" ).arg( projectGpkgPath ).arg( layerName ), layerName, "ogr" );
//  qDebug() << "!!! Layer validity check" << layer->isValid() << vl2.isValid();
//  Q_ASSERT( vl2.isValid() );

  qDebug() << "!!! SAVING Layer2" << layer->isValid() <<  projectGpkgPath << errorMessage;
  return layer;
}

void ProjectWizard::createProject( QString const &projectName )
{
  //QString projectName = QString( "TODO" ); // TODO;
  QString projectDir = InputUtils::createUniqueProjectDirectory( mDataDir, projectName );
  QString projectFilepath( QString( "%1/%2.%3" ).arg( projectDir ).arg( projectName ).arg( "qgz" ) );
  QgsVectorLayer *layer = createGpkgLayer( projectDir );

//  std::unique_ptr< QgsVectorLayer> tempLayer( new QgsVectorLayer( QStringLiteral( "none?field=code:int&field=regular:string" ), QStringLiteral( "vl" ), QStringLiteral( "memory" ) ) );
//  QVERIFY( tempLayer->isValid() );

  QgsProject project;
  project.addMapLayer( layer );
  // TODO add BG map
  project.setCrs( QgsCoordinateReferenceSystem( QStringLiteral( "EPSG:3857" ) ) );

  project.addMapLayer( layer );
  QString gpkgName( QStringLiteral( "data" ) );
  QString projectGpkgPath( QString( "%1/%2.%3" ).arg( projectDir ).arg( gpkgName ).arg( "gpkg" ) );
  // QgsProject::instance()->setFileName( fullPath.filePath() );
  project.writePath( projectGpkgPath );
  project.write( projectFilepath );

  emit projectCreated( projectDir, projectName );
}
