/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inpututils.h"

#include "qcoreapplication.h"
#include "qgsgeometrycollection.h"
#include "qgslinestring.h"
#include "qgspolygon.h"
#include "qgsvectorlayer.h"

#include "qgsquickutils.h"
#include "qgsquickmaptransform.h"
#include "inputexpressionfunctions.h"
#include "coreutils.h"

#include <Qt>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <algorithm>
#include <limits>
#include <math.h>

static const QString DATE_TIME_FORMAT = QStringLiteral( "yyMMdd-hhmmss" );

InputUtils::InputUtils( QObject *parent ): QObject( parent )
{
  mAndroidUtils = std::unique_ptr<AndroidUtils>( new AndroidUtils() );
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

  // https://github.com/lutraconsulting/input/issues/418
  // does not work for iOS files with format
  // file:assets-library://asset/asset.PNG%3Fid=A53AB989-6354-433A-9CB9-958179B7C14D&ext=PNG

  return QFile::copy( modSrcPath, dstPath );
}

QString InputUtils::getFileName( const QString &filePath )
{
  QFileInfo fileInfo( sanitizeName( filePath ) );
  return fileInfo.fileName();
}

QString InputUtils::sanitizeName( const QString &path )
{
#ifdef Q_OS_IOS
  QRegularExpression reAbs( "(.+)\\/asset\\.PNG%.Fid=(\\S+)&ext=" );
  QRegularExpressionMatch matchAbs = reAbs.match( path );
  if ( matchAbs.hasMatch() )
  {
    QString base = matchAbs.captured( 1 );
    QString name = matchAbs.captured( 2 );
    return base + "/" + name + ".png";
  }

  QRegularExpression reRel( "asset\\.PNG%.Fid=(\\S+)&ext=" );
  QRegularExpressionMatch matchRel = reRel.match( path );
  if ( matchRel.hasMatch() )
  {
    QString matched = matchRel.captured( 1 );
    return matched + ".png";
  }
#endif
  return path;
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

QString InputUtils::formatNumber( const double number, int precision )
{
  return QString::number( number, 'f', precision );
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

double InputUtils::convertCoordinateString( const QString &rationalValue )
{
  QStringList values = rationalValue.split( "," );
  if ( values.size() != 3 ) return 0;

  double degrees = ratherZeroThanNaN( convertRationalNumber( values.at( 0 ) ) );
  double minutes = ratherZeroThanNaN( convertRationalNumber( values.at( 1 ) ) );
  double seconds = ratherZeroThanNaN( convertRationalNumber( values.at( 2 ) ) );

  double result = degrees + minutes / 60 + seconds / 3600;
  return result;
}

void InputUtils::registerInputExpressionFunctions()
{
  QgsExpression::registerFunction( new ReadExif() );
  QgsExpression::registerFunction( new ReadExifImgDirection() );
  QgsExpression::registerFunction( new ReadExifLongitude() );
  QgsExpression::registerFunction( new ReadExifLatitude() );
}

double InputUtils::convertRationalNumber( const QString &rationalValue )
{
  if ( rationalValue.isEmpty() )
    return std::numeric_limits<double>::quiet_NaN();

  QStringList number = rationalValue.split( "/" );
  if ( number.size() != 2 )
    return std::numeric_limits<double>::quiet_NaN();

  double numerator = number.at( 0 ).toDouble();
  double denominator = number.at( 1 ).toDouble();
  if ( denominator == 0 )
    return denominator;

  return numerator / denominator;
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
      Q_UNUSED( e )
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

QString InputUtils::filesToString( QList<MerginFile> files )
{
  QStringList resultList;
  for ( MerginFile file : files )
  {
    resultList << file.path;
  }
  return resultList.join( ", " );
}

QString InputUtils::bytesToHumanSize( double bytes )
{
  const int precision = 1;
  if ( bytes < 1e-5 )
  {
    return "0.0";
  }
  else if ( bytes < 1024.0 * 1024.0 )
  {
    return QString::number( bytes, 'f', precision ) + " KB";
  }
  else if ( bytes < 1024.0 * 1024.0 * 1024.0 )
  {
    return QString::number( bytes / 1024.0 / 1024.0, 'f', precision ) + " MB";
  }
  else if ( bytes < 1024.0 * 1024.0 * 1024.0 * 1024.0 )
  {
    return QString::number( bytes / 1024.0 / 1024.0 / 1024.0, 'f', precision ) + " GB";
  }
  else
  {
    return QString::number( bytes / 1024.0 / 1024.0 / 1024.0 / 1024.0, 'f', precision ) + " TB";
  }
}

bool InputUtils::hasStoragePermission()
{
  if ( appPlatform() == QStringLiteral( "android" ) )
  {
    return mAndroidUtils->checkPermission( "android.permission.WRITE_EXTERNAL_STORAGE" );
  }
  return true;
}

bool InputUtils::acquireStoragePermission()
{
  if ( appPlatform() == QStringLiteral( "android" ) )
  {
    return mAndroidUtils->requestStoragePermission();
  }
  return true;
}

bool InputUtils::acquireCameraPermission()
{
  if ( appPlatform() == QStringLiteral( "android" ) )
  {
    return mAndroidUtils->requestCameraPermission();
  }
  return true;
}

void InputUtils::quitApp()
{
  QCoreApplication::quit();
}

QString InputUtils::appPlatform()
{
#if defined( ANDROID )
  const QString platform = "android";
#elif defined( Q_OS_IOS )
  const QString platform = "ios";
#elif defined( Q_OS_WIN32 )
  const QString platform = "win";
#elif defined( Q_OS_LINUX )
  const QString platform = "linux";
#elif defined( Q_OS_MAC )
  const QString platform = "macos";
#else
  const QString platform = "unknown";
#endif
  return platform;
}

void InputUtils::onQgsLogMessageReceived( const QString &message, const QString &tag, Qgis::MessageLevel level )
{
  QString levelStr;
  switch ( level )
  {
    case Qgis::MessageLevel::Warning:
      levelStr = "Warning";
      break;
    case Qgis::MessageLevel::Critical:
      levelStr = "Error";
      break;
    default:
      break;
  }

  CoreUtils::log( "QGIS " + tag, levelStr + ": " + message );
}

bool InputUtils::cpDir( const QString &srcPath, const QString &dstPath, bool onlyDiffable )
{
  bool result  = true;
  QDir parentDstDir( QFileInfo( dstPath ).path() );
  if ( !parentDstDir.mkpath( dstPath ) )
  {
    CoreUtils::log( "cpDir", QString( "Cannot make path %1" ).arg( dstPath ) );
    return false;
  }

  QDir srcDir( srcPath );
  const QFileInfoList fileInfoList = srcDir.entryInfoList( QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden );
  foreach ( const QFileInfo &info, fileInfoList )
  {
    QString srcItemPath = srcPath + "/" + info.fileName();
    QString dstItemPath = dstPath + "/" + info.fileName();
    if ( info.isDir() )
    {
      if ( !cpDir( srcItemPath, dstItemPath ) )
      {
        CoreUtils::log( "cpDir", QString( "Cannot copy a dir from %1 to %2" ).arg( srcItemPath ).arg( dstItemPath ) );
        result = false;
      }
    }
    else if ( info.isFile() )
    {
      if ( onlyDiffable && !MerginApi::isFileDiffable( info.fileName() ) )
        continue;

      if ( !QFile::copy( srcItemPath, dstItemPath ) )
      {
        if ( !QFile::remove( dstItemPath ) )
        {
          CoreUtils::log( "cpDir", QString( "Cannot remove a file from %1" ).arg( dstItemPath ) );
          result =  false;
        }
        if ( !QFile::copy( srcItemPath, dstItemPath ) )
        {
          CoreUtils::log( "cpDir", QString( "Cannot overwrite a file %1 with %2" ).arg( dstItemPath ).arg( dstItemPath ) );
          result =  false;
        }
      }
      QFile::setPermissions( dstItemPath, QFile::ReadUser | QFile::WriteUser | QFile::ReadOwner | QFile::WriteOwner );
    }
    else
    {
      CoreUtils::log( "cpDir", QString( "Unhandled item %1 in cpDir" ).arg( info.filePath() ) );
    }
  }
  return result;
}

QString InputUtils::renameWithDateTime( const QString &srcPath, const QDateTime &dateTime )
{
  if ( QFile::exists( srcPath ) )
  {
    QFileInfo info( srcPath );
    QString timestamp = ( dateTime.isValid() ) ? dateTime.toString( DATE_TIME_FORMAT ) : QDateTime::currentDateTime().toString( DATE_TIME_FORMAT );
    QString newFilename = QString( "%1.%2" ).arg( timestamp ).arg( info.suffix() );
    QString newPath( info.absolutePath() + "/" + newFilename );

    if ( QFile::rename( srcPath, newPath ) ) return newPath;
  }

  return QString();
}

void InputUtils::showNotification( const QString &message )
{
  emit showNotificationRequested( message );
}

qreal InputUtils::groundSpeedFromSource( QgsQuickPositionKit *positionKit )
{
  if ( positionKit == nullptr ) return 0;

  if ( positionKit->source()->lastKnownPosition().isValid() )
  {
    return positionKit->source()->lastKnownPosition().attribute( QGeoPositionInfo::Attribute::GroundSpeed );
  }
  return 0;
}

double InputUtils::ratherZeroThanNaN( double d )
{
  return ( isnan( d ) ) ? 0.0 : d;
}
