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
#include "positionkit.h"
#include "simulatedpositionsource.h"

#include "testutils.h"

void TestPositionKit::simulated_position()
{
  QVERIFY( !positionKit.isSimulated() );
  positionKit.useSimulatedLocation( -92.36, 38.93, -1 );
  QVERIFY( positionKit.isSimulated() );
  QVERIFY( !positionKit.satelliteSource() ); // satellite source should be NULL in this case

  QVERIFY( positionKit.hasPosition() );
  COMPARENEAR( positionKit.position().y(), 38.93, 1e-4 );
  QVERIFY( positionKit.accuracy() > 0 );

  const QVector<double> newPosition( { 90.36, 33.93, -1 } );
  positionKit.setSimulatePositionLongLatRad( newPosition );
  QVERIFY( positionKit.hasPosition() );
  COMPARENEAR( positionKit.position().y(), newPosition[1], 1e-4 );

  positionKit.setSimulatePositionLongLatRad( QVector<double>() );
  QVERIFY( !positionKit.isSimulated() );
}
