/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "iostrackingbackend.h"
#include "coreutils.h"

IOSTrackingBackend::IOSTrackingBackend( UpdateFrequency frequency, QObject *parent )
  : AbstractTrackingBackend( frequency, AbstractTrackingBackend::SignalSlotSupport::NotSupported, parent )
  , mDistanceFilter( 1 )
{
  switch ( frequency )
  {
    case AbstractTrackingBackend::Often:
      mDistanceFilter = 1;
      break;

    case AbstractTrackingBackend::Normal:
      mDistanceFilter = 5;
      break;

    case AbstractTrackingBackend::Occasional:
      mDistanceFilter = 20;
      break;
  }

  startPositionProvider( this );
}

IOSTrackingBackend::~IOSTrackingBackend()
{
  releaseObjc();
}

void IOSTrackingBackend::positionUpdate( double longitude, double latitude, double altitude )
{
  GeoPosition position;
  position.longitude = longitude;
  position.latitude = latitude;
  position.elevation = altitude;

  notifyListeners( position );
}

void IOSTrackingBackend::logError( NSString *message )
{
  CoreUtils::log( TAG, QStringLiteral( "Error:" ) + QString::fromNSString( message ) );
}

void IOSTrackingBackend::stopTrying( NSString *message )
{
  CoreUtils::log( TAG, QStringLiteral( "Stopping updates, error:" ) + QString::fromNSString( message ) );

  //
  // let user know that tracking could not start/continue for some reason
  //
}
