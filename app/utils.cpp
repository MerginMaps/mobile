/***************************************************************************
  utils.cpp
  --------------------------------------
  Date                 : Nov 2017
  Copyright            : (C) 2017 by Peter Petrik
  Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QGuiApplication>
#include <QScreen>
#include <QString>
#include <QWindow>

#include "qgis.h"
#include "qgscoordinatereferencesystem.h"
#include "qgscoordinatetransform.h"
#include "qgsdistancearea.h"
#include "qgslogger.h"
#include "qgsvectorlayer.h"
#include "qgsfeature.h"
#include "qgsapplication.h"
#include "qgsvaluerelationfieldformatter.h"
#include "qgsdatetimefieldformatter.h"

#include "featurelayerpair.h"
#include "mapsettings.h"
#include "utils.h"
#include "qgsunittypes.h"


Utils::Utils( QObject *parent )
  : QObject( parent )
  , mScreenDensity( calculateScreenDensity() )
{
}

/**
 * Makes QgsCoordinateReferenceSystem::fromEpsgId accessible for QML components
 */
QgsCoordinateReferenceSystem Utils::coordinateReferenceSystemFromEpsgId( long epsg )
{
  return QgsCoordinateReferenceSystem::fromEpsgId( epsg );
}

QgsPointXY Utils::pointXY( double x, double y )
{
  return QgsPointXY( x, y );
}

QgsPoint Utils::point( double x, double y, double z, double m )
{
  return QgsPoint( x, y, z, m );
}

QgsPoint Utils::coordinateToPoint( const QGeoCoordinate &coor )
{
  return QgsPoint( coor.longitude(), coor.latitude(), coor.altitude() );
}

QgsPointXY Utils::transformPoint( const QgsCoordinateReferenceSystem &srcCrs,
    const QgsCoordinateReferenceSystem &destCrs,
    const QgsCoordinateTransformContext &context,
    const QgsPointXY &srcPoint )
{
  try
  {
    QgsCoordinateTransform ct( srcCrs, destCrs, context );
    if ( ct.isValid() )
    {
      const QgsPointXY pt = ct.transform( srcPoint );
      return pt;
    }
  }
  catch ( QgsCsException &cse )
  {
    Q_UNUSED( cse )
  }
  return srcPoint;
}

double Utils::screenUnitsToMeters( MapSettings *mapSettings, int baseLengthPixels )
{
  if ( mapSettings == nullptr ) return 0.0;

  QgsDistanceArea mDistanceArea;
  mDistanceArea.setEllipsoid( QStringLiteral( "WGS84" ) );
  mDistanceArea.setSourceCrs( mapSettings->destinationCrs(), mapSettings->transformContext() );

  // calculate the geographic distance from the central point of extent
  // to the specified number of points on the right side
  QSize s = mapSettings->outputSize();
  QPoint pointCenter( s.width() / 2, s.height() / 2 );
  QgsPointXY p1 = mapSettings->screenToCoordinate( pointCenter );
  QgsPointXY p2 = mapSettings->screenToCoordinate( pointCenter + QPoint( baseLengthPixels, 0 ) );
  return mDistanceArea.measureLine( p1, p2 );
}

bool Utils::fileExists( const QString &path )
{
  QFileInfo check_file( path );
  // check if file exists and if yes: Is it really a file and no directory?
  return ( check_file.exists() && check_file.isFile() );
}

QString Utils::getRelativePath( const QString &path, const QString &prefixPath )
{
  QString modPath = path;
  QString filePrefix( "file://" );

  if ( path.startsWith( filePrefix ) )
  {
    modPath = modPath.replace( filePrefix, QString() );
  }

  if ( prefixPath.isEmpty() ) return modPath;

  // Do not use a canonical path for non-existing path
  if ( !QFileInfo( path ).exists() )
  {
    if ( !prefixPath.isEmpty() && modPath.startsWith( prefixPath ) )
    {
      return modPath.replace( prefixPath, QString() );
    }
  }
  else
  {
    QDir absoluteDir( modPath );
    QDir prefixDir( prefixPath );
    QString canonicalPath = absoluteDir.canonicalPath();
    QString prefixCanonicalPath = prefixDir.canonicalPath() + "/";

    if ( prefixCanonicalPath.length() > 1 && canonicalPath.startsWith( prefixCanonicalPath ) )
    {
      return canonicalPath.replace( prefixCanonicalPath, QString() );
    }
  }

  return QString();
}

void Utils::logMessage( const QString &message, const QString &tag, Qgis::MessageLevel level )
{
  QgsMessageLog::logMessage( message, tag, level );
}

FeatureLayerPair Utils::featureFactory( const QgsFeature &feature, QgsVectorLayer *layer )
{
  return FeatureLayerPair( feature, layer );
}

const QUrl Utils::getThemeIcon( const QString &name )
{
  QString path = QStringLiteral( "qrc:/%1.svg" ).arg( name );
  QgsDebugMsg( QStringLiteral( "Using icon %1 from %2" ).arg( name, path ) );
  return QUrl( path );
}

const QUrl Utils::getEditorComponentSource( const QString &widgetName )
{
  QString path( "%1.qml" );
  QStringList supportedWidgets = { QStringLiteral( "textedit" ),
                                   QStringLiteral( "valuemap" ),
                                   QStringLiteral( "valuerelation" ),
                                   QStringLiteral( "checkbox" ),
                                   QStringLiteral( "externalresource" ),
                                   QStringLiteral( "datetime" ),
                                   QStringLiteral( "range" )
                                 };
  if ( supportedWidgets.contains( widgetName ) )
  {
    return QUrl( path.arg( widgetName ) );
  }
  else
  {
    return QUrl( path.arg( QLatin1String( "textedit" ) ) );
  }
}

QString Utils::formatPoint(
  const QgsPoint &point,
  QgsCoordinateFormatter::Format format,
  int decimals,
  QgsCoordinateFormatter::FormatFlags flags )
{
  return QgsCoordinateFormatter::format( point, format, decimals, flags );
}

QString Utils::formatDistance( double distance,
                                       QgsUnitTypes::DistanceUnit units,
                                       int decimals,
                                       QgsUnitTypes::SystemOfMeasurement destSystem )
{
  double destDistance;
  QgsUnitTypes::DistanceUnit destUnits;

  humanReadableDistance( distance, units, destSystem, destDistance, destUnits );

  return QStringLiteral( "%1 %2" )
         .arg( QString::number( destDistance, 'f', decimals ) )
         .arg( QgsUnitTypes::toAbbreviatedString( destUnits ) );
}

bool Utils::removeFile( const QString &filePath )
{
  QFile file( filePath );
  return file.remove( filePath );
}


void Utils::humanReadableDistance( double srcDistance, QgsUnitTypes::DistanceUnit srcUnits,
    QgsUnitTypes::SystemOfMeasurement destSystem,
    double &destDistance, QgsUnitTypes::DistanceUnit &destUnits )
{
  if ( ( destSystem == QgsUnitTypes::MetricSystem ) || ( destSystem == QgsUnitTypes::UnknownSystem ) )
  {
    return formatToMetricDistance( srcDistance, srcUnits, destDistance, destUnits );
  }
  else if ( destSystem == QgsUnitTypes::ImperialSystem )
  {
    return formatToImperialDistance( srcDistance, srcUnits, destDistance, destUnits );
  }
  else if ( destSystem == QgsUnitTypes::USCSSystem )
  {
    return formatToUSCSDistance( srcDistance, srcUnits, destDistance, destUnits );
  }
  else
  {
    Q_ASSERT( false ); //should never happen
  }
}

void Utils::formatToMetricDistance( double srcDistance,
    QgsUnitTypes::DistanceUnit srcUnits,
    double &destDistance,
    QgsUnitTypes::DistanceUnit &destUnits )
{
  double dist = srcDistance * QgsUnitTypes::fromUnitToUnitFactor( srcUnits, QgsUnitTypes::DistanceMillimeters );
  if ( dist < 0 )
  {
    destDistance = 0;
    destUnits = QgsUnitTypes::DistanceMillimeters;
    return;
  }

  double mmToKm = QgsUnitTypes::fromUnitToUnitFactor( QgsUnitTypes::DistanceKilometers, QgsUnitTypes::DistanceMillimeters );
  if ( dist > mmToKm )
  {
    destDistance = dist / mmToKm;
    destUnits = QgsUnitTypes::DistanceKilometers;
    return;
  }

  double mmToM = QgsUnitTypes::fromUnitToUnitFactor( QgsUnitTypes::DistanceMeters, QgsUnitTypes::DistanceMillimeters );
  if ( dist > mmToM )
  {
    destDistance = dist / mmToM;
    destUnits = QgsUnitTypes::DistanceMeters;
    return;
  }

  double mmToCm = QgsUnitTypes::fromUnitToUnitFactor( QgsUnitTypes::DistanceCentimeters, QgsUnitTypes::DistanceMillimeters );
  if ( dist > mmToCm )
  {
    destDistance = dist / mmToCm;
    destUnits = QgsUnitTypes::DistanceCentimeters;
    return;
  }

  destDistance = dist;
  destUnits = QgsUnitTypes::DistanceMillimeters;
}

void Utils::formatToImperialDistance( double srcDistance,
    QgsUnitTypes::DistanceUnit srcUnits,
    double &destDistance,
    QgsUnitTypes::DistanceUnit &destUnits )
{
  double dist = srcDistance * QgsUnitTypes::fromUnitToUnitFactor( srcUnits, QgsUnitTypes::DistanceFeet );
  if ( dist < 0 )
  {
    destDistance = 0;
    destUnits = QgsUnitTypes::DistanceFeet;
    return;
  }

  double feetToMile = QgsUnitTypes::fromUnitToUnitFactor( QgsUnitTypes::DistanceMiles, QgsUnitTypes::DistanceFeet );
  if ( dist > feetToMile )
  {
    destDistance = dist / feetToMile;
    destUnits = QgsUnitTypes::DistanceMiles;
    return;
  }

  double feetToYard = QgsUnitTypes::fromUnitToUnitFactor( QgsUnitTypes::DistanceYards, QgsUnitTypes::DistanceFeet );
  if ( dist > feetToYard )
  {
    destDistance = dist / feetToYard;
    destUnits = QgsUnitTypes::DistanceYards;
    return;
  }

  destDistance = dist;
  destUnits = QgsUnitTypes::DistanceFeet;
  return;
}

void Utils::formatToUSCSDistance( double srcDistance,
    QgsUnitTypes::DistanceUnit srcUnits,
    double &destDistance,
    QgsUnitTypes::DistanceUnit &destUnits )
{
  double dist = srcDistance * QgsUnitTypes::fromUnitToUnitFactor( srcUnits, QgsUnitTypes::DistanceFeet );
  if ( dist < 0 )
  {
    destDistance = 0;
    destUnits = QgsUnitTypes::DistanceFeet;
    return;
  }

  double feetToMile = QgsUnitTypes::fromUnitToUnitFactor( QgsUnitTypes::DistanceNauticalMiles, QgsUnitTypes::DistanceFeet );
  if ( dist > feetToMile )
  {
    destDistance = dist / feetToMile;
    destUnits = QgsUnitTypes::DistanceNauticalMiles;
    return;
  }

  double feetToYard = QgsUnitTypes::fromUnitToUnitFactor( QgsUnitTypes::DistanceYards, QgsUnitTypes::DistanceFeet );
  if ( dist > feetToYard )
  {
    destDistance = dist / feetToYard;
    destUnits = QgsUnitTypes::DistanceYards;
    return;
  }

  destDistance = dist;
  destUnits = QgsUnitTypes::DistanceFeet;
  return;
}

QString Utils::dumpScreenInfo() const
{
  QString msg;
  // take the first top level window
  const QWindowList windows = QGuiApplication::topLevelWindows();
  if ( !windows.isEmpty() )
  {
    QScreen *screen = windows.at( 0 )->screen();
    double dpiX = screen->physicalDotsPerInchX();
    double dpiY = screen->physicalDotsPerInchY();
    int height = screen->geometry().height();
    int width = screen->geometry().width();
    double sizeX = static_cast<double>( width ) / dpiX * 25.4;
    double sizeY = static_cast<double>( height ) / dpiY * 25.4;

    msg += tr( "screen resolution: %1x%2 px\n" ).arg( width ).arg( height );
    msg += tr( "screen DPI: %1x%2\n" ).arg( dpiX ).arg( dpiY );
    msg += tr( "screen size: %1x%2 mm\n" ).arg( QString::number( sizeX, 'f', 0 ), QString::number( sizeY, 'f', 0 ) );
    msg += tr( "screen density: %1" ).arg( mScreenDensity );
  }
  else
  {
    msg += QLatin1String( "screen info: application is not initialized!" );
  }
  return msg;
}

QVariantMap Utils::createValueRelationCache( const QVariantMap &config, const QgsFeature &formFeature )
{
  QVariantMap valueMap;
  QgsValueRelationFieldFormatter::ValueRelationCache cache = QgsValueRelationFieldFormatter::createCache( config, formFeature );

  for ( const QgsValueRelationFieldFormatter::ValueRelationItem &item : qgis::as_const( cache ) )
  {
    valueMap.insert( item.key.toString(), item.value );
  }
  return valueMap;
}

QString Utils::evaluateExpression( const FeatureLayerPair &pair, QgsProject *activeProject, const QString &expression )
{
  QList<QgsExpressionContextScope *> scopes;
  scopes << QgsExpressionContextUtils::globalScope();
  scopes << QgsExpressionContextUtils::projectScope( activeProject );
  scopes << QgsExpressionContextUtils::layerScope( pair.layer() );

  QgsExpressionContext context( scopes );
  context.setFeature( pair.feature() );
  QgsExpression expr( expression );
  return expr.evaluate( &context ).toString();
}

void Utils::selectFeaturesInLayer( QgsVectorLayer *layer, const QList<int> &fids, QgsVectorLayer::SelectBehavior behavior )
{
  QgsFeatureIds qgsFids;
  for ( const int &fid : fids )
    qgsFids << fid;
  layer->selectByIds( qgsFids, behavior );
}

QString Utils::fieldType( const QgsField &field )
{
  return QVariant( field.type() ).typeName();
}

QString Utils::dateTimeFieldFormat( const QString &fieldFormat )
{
  if ( QgsDateTimeFieldFormatter::DATE_FORMAT == fieldFormat )
  {
    return QString( "Date" );
  }
  else if ( QgsDateTimeFieldFormatter::TIME_FORMAT == fieldFormat )
  {
    return QString( "Time" );
  }
  else if ( QgsDateTimeFieldFormatter::DATETIME_FORMAT == fieldFormat )
  {
    return QString( "Date Time" );
  }
  else
  {
    return QString( "Date Time" );
  }
}

QModelIndex Utils::invalidIndex()
{
  return QModelIndex();
}

qreal Utils::screenDensity() const
{
  return mScreenDensity;
}

qreal Utils::calculateScreenDensity()
{
  // calculate screen density for calculation of real pixel sizes from density-independent pixels
  // take the first top level window
  double dpi = 96.0;
  const QWindowList windows = QGuiApplication::topLevelWindows();
  if ( !windows.isEmpty() )
  {
    QScreen *screen = windows.at( 0 )->screen();
    double dpiX = screen->physicalDotsPerInchX();
    double dpiY = screen->physicalDotsPerInchY();
    dpi = dpiX < dpiY ? dpiX : dpiY; // In case of asymmetrical DPI. Improbable
  }
  return dpi / 160.;  // 160 DPI is baseline for density-independent pixels in Android
}
