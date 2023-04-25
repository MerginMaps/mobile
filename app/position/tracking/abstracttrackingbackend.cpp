/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "abstracttrackingbackend.h"
#include "coreutils.h"

AbstractTrackingBackend::AbstractTrackingBackend( UpdateFrequency updateFrequency, SignalSlotSupport signalSlotSupport, QObject *parent )
  : QObject{ parent }
  , mUpdateFrequency( updateFrequency )
  , mSignalSlotSupport( signalSlotSupport )
{

}

void AbstractTrackingBackend::notifyListeners( const GeoPosition &position )
{
  if ( mSignalSlotSupport == SignalSlotSupport::Supported )
  {
    emit positionChanged( position );
    return;
  }

  if ( mNotifyFunction )
  {
    mNotifyFunction( position );
  }
  else
  {
    CoreUtils::log( QStringLiteral( "Tracking backend" ), QStringLiteral( "No way to inform about position update!" ) );
  }
}

AbstractTrackingBackend::UpdateFrequency AbstractTrackingBackend::updateFrequency() const
{
  return mUpdateFrequency;
}

void AbstractTrackingBackend::setUpdateFrequency( const UpdateFrequency &newUpdateFrequency )
{
  if ( mUpdateFrequency == newUpdateFrequency )
    return;
  mUpdateFrequency = newUpdateFrequency;
  emit updateFrequencyChanged( mUpdateFrequency );
}

AbstractTrackingBackend::SignalSlotSupport AbstractTrackingBackend::signalSlotSupport() const
{
  return mSignalSlotSupport;
}

void AbstractTrackingBackend::setNotifyFunction( std::function<void ( const GeoPosition & )> fn )
{
  mNotifyFunction = fn;
}

void AbstractTrackingBackend::setSignalSlotSupport( SignalSlotSupport support )
{
  mSignalSlotSupport = support;
}
