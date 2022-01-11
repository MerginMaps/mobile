/***************************************************************************
     testqgspositionkit.cpp
     --------------------------------------
  Date                 : May 2018
  Copyright            : (C) 2017 by Viktor Sklencar
  Email                : vsklencar at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "testpositionkit.h"

#include <QObject>
#include <QApplication>
#include <QDesktopWidget>

#include "qgsapplication.h"
#include "position/positionkit.h"
#include "position/simulatedpositionprovider.h"

#include "testutils.h"

void TestPositionKit::simulated_position()
{
  QVERIFY( !positionKit.positionProvider() );

  SimulatedPositionProvider simulatedProvider( -92.36, 38.93, 0 );
  simulatedProvider.startUpdates();

  positionKit.setPositionProvider( &simulatedProvider );

  QVERIFY( positionKit.positionProvider() );

  QVERIFY( positionKit.hasPosition() );

  COMPARENEAR( positionKit.position().y(), 38.93, 1e-4 );
  QVERIFY( positionKit.accuracy() > 0 );
  QVERIFY( positionKit.satellitesInViewCount() >= 0 );
  QVERIFY( positionKit.usedSatellitesCount() >= 0 );

  SimulatedPositionProvider simulatedProvider2( 90.36, 33.93, 0 );
  simulatedProvider2.startUpdates();

  positionKit.setPositionProvider( &simulatedProvider2 );

  QVERIFY( positionKit.hasPosition() );
  COMPARENEAR( positionKit.position().y(), 33.93, 1e-4 );

  simulatedProvider.stopUpdates();
  simulatedProvider2.stopUpdates();

  positionKit.setPositionProvider( nullptr );
  QVERIFY( !positionKit.positionProvider() );
}
