/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "positionkit.h"
#include "coreutils.h"

#include "qgis.h"

#ifndef Q_OS_IOS
#include "bluetoothpositionprovider.h"
#endif

#include "internalpositionprovider.h"
#include "simulatedpositionprovider.h"

#include "appsettings.h"
#include "inpututils.h"

#include <QQmlEngine>

PositionKit::PositionKit( QObject *parent )
  : QObject( parent )
{
}

QgsCoordinateReferenceSystem PositionKit::positionCRS() const
{
  return QgsCoordinateReferenceSystem::fromEpsgId( 4326 );
}

void PositionKit::startUpdates()
{
  if ( mPositionProvider )
    mPositionProvider->startUpdates();
}

void PositionKit::stopUpdates()
{
  if ( mPositionProvider )
    mPositionProvider->stopUpdates();
}

void PositionKit::setPositionProvider( AbstractPositionProvider *provider )
{
  if ( mPositionProvider.get() == provider )
    return;

  if ( mPositionProvider.get() && provider && mPositionProvider->id() == provider->id() )
    return;

  if ( mPositionProvider )
  {
    mPositionProvider->disconnect();
    mPositionProvider->closeProvider();
  }

  mPositionProvider.reset( provider );

  if ( mPositionProvider )
  {
    connect( mPositionProvider.get(), &AbstractPositionProvider::positionChanged, this, &PositionKit::parsePositionUpdate );

    CoreUtils::log( QStringLiteral( "PositionKit" ), QStringLiteral( "Changed position provider to: %1" ).arg( provider->id() ) );
  }
  else // passed nullptr
  {
    CoreUtils::log( QStringLiteral( "PositionKit" ), QStringLiteral( "Position provider has been removed" ) );
  }

  emit positionProviderChanged( provider );

  // reset last position data
  parsePositionUpdate( GeoPosition() );
}

AbstractPositionProvider *PositionKit::constructProvider( const QString &type, const QString &id, const QString &name )
{
  QString providerType( type );

  // currently the only external provider is bluetooth, so manually set internal provider for platforms that
  // do not support reading bluetooth serial
#ifndef HAVE_BLUETOOTH
  providerType = QStringLiteral( "internal" );
#endif

  if ( type == QStringLiteral( "external" ) )
  {
#ifdef HAVE_BLUETOOTH
    AbstractPositionProvider *provider = new BluetoothPositionProvider( id, name );
    QQmlEngine::setObjectOwnership( provider, QQmlEngine::CppOwnership );
    return provider;
#endif
  }
  else // type == internal
  {
    if ( id == QStringLiteral( "simulated" ) )
    {
      AbstractPositionProvider *provider = new SimulatedPositionProvider();
      QQmlEngine::setObjectOwnership( provider, QQmlEngine::CppOwnership );
      return provider;
    }
    else // id == devicegps
    {
      AbstractPositionProvider *provider = new InternalPositionProvider();
      QQmlEngine::setObjectOwnership( provider, QQmlEngine::CppOwnership );
      return provider;
    }
  }
}

AbstractPositionProvider *PositionKit::constructActiveProvider( AppSettings *appsettings )
{
  if ( !appsettings )
    return nullptr;

  QString providerId = appsettings->activePositionProviderId();

  if ( providerId.isEmpty() ) // nothing has been written to qsettings
  {
    if ( InputUtils::isMobilePlatform() )
    {
      return constructProvider( QStringLiteral( "internal" ), QStringLiteral( "devicegps" ) );
    }
    else // desktop
    {
      return constructProvider( QStringLiteral( "internal" ), QStringLiteral( "simulated" ) );
    }
  }
  else if ( providerId == QStringLiteral( "devicegps" ) )
  {
    return constructProvider( QStringLiteral( "internal" ), QStringLiteral( "devicegps" ) );
  }
  else if ( providerId == QStringLiteral( "simulated" ) )
  {
    return constructProvider( QStringLiteral( "internal" ), QStringLiteral( "simulated" ) );
  }
  else
  {
    // find name of the active provider
    QString providerName;
    QVariantList providers = appsettings->savedPositionProviders();

    for ( const auto &provider : providers )
    {
      QVariantList providerData = provider.toList();
      if ( providerData.length() < 2 )
      {
        CoreUtils::log( QStringLiteral( "PositionKit" ), QStringLiteral( "Found provider with insufficient data" ) );
        continue;
      }

      if ( providerData[1] == providerId )
      {
        providerName = providerData[0].toString();
      }
    }

    return constructProvider( QStringLiteral( "external" ), providerId, providerName );
  }
}

void PositionKit::parsePositionUpdate( const GeoPosition &newPosition )
{
  bool hasAnythingChanged = false;

  if ( !qgsDoubleNear( newPosition.latitude, mPosition.latitude ) )
  {
    mPosition.latitude = newPosition.latitude;
    emit latitudeChanged( mPosition.latitude );
    emit positionCoordinateChanged( positionCoordinate() );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.longitude, mPosition.longitude ) )
  {
    mPosition.longitude = newPosition.longitude;
    emit longitudeChanged( mPosition.longitude );
    emit positionCoordinateChanged( positionCoordinate() );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.elevation, mPosition.elevation ) )
  {
    mPosition.elevation = newPosition.elevation;
    emit altitudeChanged( mPosition.elevation );
    hasAnythingChanged = true;
  }

  if ( newPosition.hasValidPosition() != mHasPosition )
  {
    mHasPosition = newPosition.hasValidPosition();
    emit hasPositionChanged( mHasPosition );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.hacc, mPosition.hacc ) )
  {
    mPosition.hacc = newPosition.hacc;
    emit horizontalAccuracyChanged( mPosition.hacc );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.vacc, mPosition.vacc ) )
  {
    mPosition.vacc = newPosition.vacc;
    emit verticalAccuracyChanged( mPosition.vacc );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.speed, mPosition.speed ) )
  {
    mPosition.speed = newPosition.speed;
    emit speedChanged( mPosition.speed );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.verticalSpeed, mPosition.verticalSpeed ) )
  {
    mPosition.verticalSpeed = newPosition.verticalSpeed;
    emit verticalSpeedChanged( mPosition.verticalSpeed );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.direction, mPosition.direction ) )
  {
    mPosition.direction = newPosition.direction;
    emit directionChanged( mPosition.direction );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.magneticVariation, mPosition.magneticVariation ) )
  {
    mPosition.magneticVariation = newPosition.magneticVariation;
    emit magneticVariationChanged( mPosition.magneticVariation );
    hasAnythingChanged = true;
  }

  if ( newPosition.satellitesVisible != mPosition.satellitesVisible )
  {
    mPosition.satellitesVisible = newPosition.satellitesVisible;
    emit satellitesVisibleChanged( mPosition.satellitesVisible );
    hasAnythingChanged = true;
  }

  if ( newPosition.satellitesUsed != mPosition.satellitesUsed )
  {
    mPosition.satellitesUsed = newPosition.satellitesUsed;
    emit satellitesUsedChanged( mPosition.satellitesUsed );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.hdop, mPosition.hdop ) )
  {
    mPosition.hdop = newPosition.hdop;
    emit hdopChanged( mPosition.hdop );
    hasAnythingChanged = true;
  }

  if ( newPosition.fixStatusString != mPosition.fixStatusString )
  {
    mPosition.fixStatusString = newPosition.fixStatusString;
    emit fixChanged( mPosition.fixStatusString );
    hasAnythingChanged = true;
  }

  if ( newPosition.quality != mPosition.quality )
  {
    mPosition.quality = newPosition.quality;
    hasAnythingChanged = true;
  }

  if ( newPosition.utcDateTime != mPosition.utcDateTime )
  {
    mPosition.utcDateTime = newPosition.utcDateTime;
    emit lastReadChanged( mPosition.utcDateTime );
    hasAnythingChanged = true;
  }

  if ( hasAnythingChanged )
  {
    emit positionChanged( mPosition );
  }
}

void PositionKit::appStateChanged( Qt::ApplicationState state )
{
  if ( state == Qt::ApplicationActive )
  {
    startUpdates();
  }
  else
  {
    stopUpdates();
  }
}

double PositionKit::latitude() const
{
  return mPosition.latitude;
}

double PositionKit::longitude() const
{
  return mPosition.longitude;
}

double PositionKit::altitude() const
{
  return mPosition.elevation;
}

QgsPoint PositionKit::positionCoordinate() const
{
  if ( mPosition.hasValidPosition() )
    return QgsPoint( mPosition.longitude, mPosition.latitude, mPosition.elevation );

  return QgsPoint();
}

bool PositionKit::hasPosition() const
{
  return mHasPosition;
}

double PositionKit::horizontalAccuracy() const
{
  return mPosition.hacc;
}

double PositionKit::verticalAccuracy() const
{
  return mPosition.vacc;
}

double PositionKit::direction() const
{
  return mPosition.direction;
}

double PositionKit::magneticVariation() const
{
  return mPosition.magneticVariation;
}

double PositionKit::speed() const
{
  return mPosition.speed;
}

double PositionKit::verticalSpeed() const
{
  return mPosition.verticalSpeed;
}

const QDateTime &PositionKit::lastRead() const
{
  return mPosition.utcDateTime;
}

int PositionKit::satellitesUsed() const
{
  return mPosition.satellitesUsed;
}

int PositionKit::satellitesVisible() const
{
  return mPosition.satellitesVisible;
}

QString PositionKit::fix() const
{
  return mPosition.fixStatusString;
}

double PositionKit::hdop() const
{
  return mPosition.hdop;
}

AbstractPositionProvider *PositionKit::positionProvider() const
{
  return mPositionProvider.get();
}

const GeoPosition &PositionKit::position() const
{
  return mPosition;
}
