#include "inpututils.h"

#include "qgsgeometrycollection.h"
#include "qgslinestring.h"
#include "qgspolygon.h"
#include "qgsvectorlayer.h"

#include "qgsquickmaptransform.h"

#include <QFile>
#include <QFileInfo>


InputUtils::InputUtils( QObject *parent ): QObject( parent )
{
}

bool InputUtils::removeFile( const QString &filePath )
{
  QFile file( filePath );
  return file.remove( filePath );
}

bool InputUtils::copyFile( const QString &srcPath, const QString &dstPath )
{
  QString modSrcPath = srcPath;
  if ( srcPath.startsWith( "file://" ) )
  {
    modSrcPath = modSrcPath.replace( "file://", "" );
  }

  return QFile::copy( modSrcPath, dstPath );
}

QString InputUtils::getFileName( const QString &filePath )
{
  QFileInfo fileInfo( filePath );
  return fileInfo.fileName();
}

QString InputUtils::formatProjectName( const QString &fullProjectName )
{
  if ( fullProjectName.contains( "/" ) )
  {
    QStringList list = fullProjectName.split( "/" );
    if ( list.at( 0 ).isEmpty() )
    {
      return QString( "<b>%1</b>" ).arg( list.at( 1 ) );
    }
    else
    {
      return QString( "%1/<b>%2</b>" ).arg( list.at( 0 ) ).arg( list.at( 1 ) );
    }
  }
  else
  {
    return QString( "<b>%1</b>" ).arg( fullProjectName );
  }
}

void InputUtils::setExtentToFeature( const QgsQuickFeatureLayerPair &pair, QgsQuickMapSettings *mapSettings, double panelOffsetRatio )
{

  if ( !mapSettings )
    return;

  if ( !pair.layer() )
    return;

  if ( !pair.feature().isValid() )
    return;

  QgsGeometry geom = pair.feature().geometry();
  if ( geom.isNull() || !geom.constGet() )
    return;

  QgsRectangle bbox = mapSettings->mapSettings().layerExtentToOutputExtent( pair.layer(), geom.boundingBox() );
  QgsRectangle currentExtent = mapSettings->mapSettings().extent();
  QgsPointXY currentExtentCenter = currentExtent.center();
  QgsPointXY featureCenter = bbox.center();

  double panelOffset = ( currentExtent.yMaximum() - currentExtent.yMinimum() ) * panelOffsetRatio / 2;
  double offsetX = currentExtentCenter.x() - featureCenter.x();
  double offsetY = currentExtentCenter.y() - featureCenter.y();
  currentExtent.setXMinimum( currentExtent.xMinimum() - offsetX );
  currentExtent.setXMaximum( currentExtent.xMaximum() - offsetX );
  currentExtent.setYMinimum( currentExtent.yMinimum() - offsetY - panelOffset );
  currentExtent.setYMaximum( currentExtent.yMaximum() - offsetY - panelOffset );
  mapSettings->setExtent( currentExtent );
}


double InputUtils::mapSettingsScale( QgsQuickMapSettings *ms )
{
  if ( !ms ) return 1;
  return 1 / ms->mapUnitsPerPixel();
}

double InputUtils::mapSettingsOffsetX( QgsQuickMapSettings *ms )
{
  if ( !ms ) return 0;
  return -ms->visibleExtent().xMinimum();
}

double InputUtils::mapSettingsOffsetY( QgsQuickMapSettings *ms )
{
  if ( !ms ) return 0;
  return -ms->visibleExtent().yMaximum();
}


static void addLineString( const QgsLineString *line, QVector<double> &data )
{
  data << line->numPoints();
  const double *x = line->xData();
  const double *y = line->yData();
  for ( int i = 0; i < line->numPoints(); ++i )
  {
    data << x[i] << y[i];
  }
}

static void addSingleGeometry( const QgsAbstractGeometry *geom, QgsWkbTypes::GeometryType type, QVector<double> &data )
{
  switch ( type )
  {
    case QgsWkbTypes::PointGeometry:
    {
      const QgsPoint *point = qgsgeometry_cast<const QgsPoint *>( geom );
      if ( point )
      {
        data << 0 << point->x() << point->y();
      }
      break;
    }

    case QgsWkbTypes::LineGeometry:
    {
      const QgsLineString *line = qgsgeometry_cast<const QgsLineString *>( geom );
      if ( line )
      {
        data << 1;
        addLineString( line, data );
      }
      break;
    }

    case QgsWkbTypes::PolygonGeometry:
    {
      const QgsPolygon *poly = qgsgeometry_cast<const QgsPolygon *>( geom );
      if ( poly )
      {
        if ( const QgsLineString *line = qgsgeometry_cast<const QgsLineString *>( poly->exteriorRing() ) )
        {
          data << 2;
          addLineString( line, data );
        }
        for ( int i = 0; i < poly->numInteriorRings(); ++i )
        {
          if ( const QgsLineString *line = qgsgeometry_cast<const QgsLineString *>( poly->interiorRing( i ) ) )
          {
            data << 2;
            addLineString( line, data );
          }
        }
      }
      break;
    }

    case QgsWkbTypes::UnknownGeometry:
    case QgsWkbTypes::NullGeometry:
      break;
  }
}


QVector<double> InputUtils::extractGeometryCoordinates( const QgsQuickFeatureLayerPair &pair, QgsQuickMapSettings *mapSettings )
{
  if ( !mapSettings || !pair.isValid() )
    return QVector<double>();

  QgsGeometry g = pair.feature().geometry();

  QgsCoordinateTransform ct( pair.layer()->crs(), mapSettings->destinationCrs(), mapSettings->transformContext() );
  if ( !ct.isShortCircuited() )
  {
    try
    {
      g.transform( ct );
    }
    catch ( QgsCsException &e )
    {
      Q_UNUSED( e );
      return QVector<double>();
    }
  }

  QVector<double> data;

  const QgsAbstractGeometry *geom = g.constGet();
  QgsWkbTypes::GeometryType geomType = g.type();
  const QgsGeometryCollection *collection = qgsgeometry_cast<const QgsGeometryCollection *>( geom );
  if ( collection && !collection->isEmpty() )
  {
    for ( int i = 0; i < collection->numGeometries(); ++i )
    {
      addSingleGeometry( collection->geometryN( i ), geomType, data );
    }
  }
  else
  {
    addSingleGeometry( geom, geomType, data );
  }

  return data;
}

bool InputUtils::cpDir( const QString &srcPath, const QString &dstPath )
{
  QDir parentDstDir( QFileInfo( dstPath ).path() );
  if ( !parentDstDir.mkpath( dstPath ) )
    return false;

  QDir srcDir( srcPath );
  foreach ( const QFileInfo &info, srcDir.entryInfoList( QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden ) )
  {
    QString srcItemPath = srcPath + "/" + info.fileName();
    QString dstItemPath = dstPath + "/" + info.fileName();
    if ( info.isDir() )
    {
      if ( !cpDir( srcItemPath, dstItemPath ) )
      {
        return false;
      }
    }
    else if ( info.isFile() )
    {
      if ( !QFile::copy( srcItemPath, dstItemPath ) )
      {
        return false;
      }
      QFile::setPermissions( dstItemPath, QFile::ReadUser | QFile::WriteUser | QFile::ReadOwner | QFile::WriteOwner );
    }
    else
    {
      qDebug() << "Unhandled item" << info.filePath() << "in cpDir";
    }
  }
  return true;
}
