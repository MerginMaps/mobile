/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "internaltrackingbackend.h"
#include "positionkit.h"

InternalTrackingBackend::InternalTrackingBackend(
  PositionKit *positionKit,
  UpdateFrequency updateFrequency,
  QObject *parent )
  : AbstractTrackingBackend(
      updateFrequency,
      parent
    )
  , mLastUpdate( QDateTime::currentDateTime() )
  , mPositionKit( positionKit )
{
  switch ( updateFrequency )
  {
    case UpdateFrequency::Often:
      mUpdateInterval = 1000;
      break;
    case UpdateFrequency::Normal:
      mUpdateInterval = 2000;
      break;
    case UpdateFrequency::Occasional:
      mUpdateInterval = 5000;
      break;
  }

  if ( mPositionKit )
  {
    connect( mPositionKit, &PositionKit::positionChanged, this, [this]( GeoPosition position )
    {
      if ( mLastUpdate.addMSecs( mUpdateInterval ) <= QDateTime::currentDateTime() )
      {
        storeDataAndNotify( position.longitude, position.latitude, position.elevation, QDateTime::currentDateTime().toSecsSinceEpoch() );
      }
    } );
  }
}
