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

void TestPositionKit::init()
{
  qRegisterMetaType< GeoPosition >( "GeoPosition" );
}

void TestPositionKit::simulated_position()
{
  QVERIFY( !positionKit.positionProvider() );

  SimulatedPositionProvider *simulatedProvider = new SimulatedPositionProvider( -92.36, 38.93, 0 );

  positionKit.setPositionProvider( simulatedProvider ); // ownership of provider is passed to positionkit

  QVERIFY( positionKit.positionProvider() );
  simulatedProvider = nullptr;

  QSignalSpy positionKitSpy( &positionKit, &PositionKit::positionChanged );
  bool hasPositionChanged = positionKitSpy.wait( 2000 );

  QVERIFY( hasPositionChanged );
  QVERIFY( positionKit.hasPosition() );

  COMPARENEAR( positionKit.positionCoordinate().y(), 38.93, 1e-4 );
  QVERIFY( positionKit.horizontalAccuracy() > 0 );
  QVERIFY( positionKit.satellitesVisible() >= 0 );
  QVERIFY( positionKit.satellitesUsed() >= 0 );

  SimulatedPositionProvider *simulatedProvider2 = new SimulatedPositionProvider( 90.36, 33.93, 0 );

  positionKit.setPositionProvider( simulatedProvider2 ); // deletes the first provider
  simulatedProvider2 = nullptr;

  hasPositionChanged = positionKitSpy.wait( 2000 );

  QVERIFY( hasPositionChanged );
  QVERIFY( positionKit.hasPosition() );
  COMPARENEAR( positionKit.positionCoordinate().y(), 33.93, 1e-4 );

  positionKit.stopUpdates();

  positionKit.setPositionProvider( nullptr );
  QVERIFY( !positionKit.positionProvider() );
}
