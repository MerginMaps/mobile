/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "simulatedtrackingbackend.h"

#include "position/providers/abstractpositionprovider.h"
#include "position/providers/simulatedpositionprovider.h"

SimulatedTrackingBackend::SimulatedTrackingBackend(
  AbstractPositionProvider *positionProvider,
  UpdateFrequency updateFrequency,
  QObject *parent )
  : AbstractTrackingBackend{parent}
{
  mPositionProvider.reset( positionProvider );

  switch ( updateFrequency )
  {
    case UpdateFrequency::Often:
      mPositionProvider->setUpdateInterval( 1000 );
      break;
    case UpdateFrequency::Normal:
      mPositionProvider->setUpdateInterval( 2000 );
      break;
    case UpdateFrequency::Occasional:
      mPositionProvider->setUpdateInterval( 5000 );
      break;
  }

  connect( mPositionProvider.get(), &AbstractPositionProvider::positionChanged, this, &AbstractTrackingBackend::positionChanged );
}

SimulatedTrackingBackend::SimulatedTrackingBackend( UpdateFrequency updateFrequency, QObject *parent )
  : AbstractTrackingBackend{parent}
{

  double longitude = 17.107137342092614;
  double latitude = 48.10301740375036;
  double flightRadius = 0.1;
  double updateTimeout = 1000;

  switch ( updateFrequency )
  {
    case UpdateFrequency::Often:
      updateTimeout = 1000;
      break;
    case UpdateFrequency::Normal:
      updateTimeout = 2000;
      break;
    case UpdateFrequency::Occasional:
      updateTimeout = 5000;
      break;
  }

  mPositionProvider = std::make_unique<SimulatedPositionProvider>( longitude, latitude, flightRadius, updateTimeout );

  connect( mPositionProvider.get(), &AbstractPositionProvider::positionChanged, this, &AbstractTrackingBackend::positionChanged );
}
