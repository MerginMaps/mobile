/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "abstracttrackingbackend.h"

AbstractTrackingBackend::AbstractTrackingBackend(
  UpdateFrequency updateFrequency,
  SignalSlotSupport signalSlotSupport,
  TrackingMethod trackingMethod,
  QObject *parent
)
  : QObject( parent )
  , mUpdateFrequency( updateFrequency )
  , mTrackingMethod( trackingMethod )
  , mSignalSlotSupport( signalSlotSupport )
{

}

void AbstractTrackingBackend::notifyListeners( const QgsPoint &position )
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

AbstractTrackingBackend::TrackingMethod AbstractTrackingBackend::trackingMethod() const
{
  return mTrackingMethod;
}

void AbstractTrackingBackend::setNotifyFunction( std::function<void ( const QgsPoint & )> fn )
{
  mNotifyFunction = fn;
}

void AbstractTrackingBackend::setSignalSlotSupport( SignalSlotSupport support )
{
  mSignalSlotSupport = support;
}
