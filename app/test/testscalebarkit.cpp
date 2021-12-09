/***************************************************************************
     testscalebarkit.cpp
     --------------------------------------
  Date                 : May 2018
  Copyright            : (C) 2018 by Viktor Sklencar
  Email                : vsklencar at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "testscalebarkit.h"

#include <QObject>
#include <QApplication>
#include <QDesktopWidget>

#include "qgsapplication.h"
#include "qgsquickmapcanvasmap.h"
#include "scalebarkit.h"

#include "testutils.h"

void TestScaleBarKit::updateScaleBar()
{
  QgsCoordinateReferenceSystem crsGPS = QgsCoordinateReferenceSystem::fromEpsgId( 4326 );
  QVERIFY( crsGPS.authid() == "EPSG:4326" );

  QgsRectangle extent = QgsRectangle( 49, 16, 50, 17 );
  QgsQuickMapCanvasMap canvas;

  QgsQuickMapSettings *ms = canvas.mapSettings();
  ms->setDestinationCrs( crsGPS );
  ms->setExtent( extent );
  ms->setOutputSize( QSize( 1000, 500 ) );

  ScaleBarKit kit;
  kit.setMapSettings( ms );
  QCOMPARE( kit.units(), "km" );
  COMPARENEAR( kit.distance(), 50, 1 );

  qreal scale = 0.005;
  canvas.zoom( extent.center().toQPointF(), scale );
  QCOMPARE( kit.units(), "m" );
  COMPARENEAR( kit.distance(), 200, 1 );
}
