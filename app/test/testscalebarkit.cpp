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
#include "qgsquickutils.h"
#include "qgsquickmapcanvasmap.h"
#include "qgsquickscalebarkit.h"


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

  QgsQuickScaleBarKit kit;
  kit.setMapSettings( ms );
  QVERIFY( kit.units() == QString( "km" ) );
  QVERIFY( kit.distance() == 50 );

  qreal scale = 0.005;
  canvas.zoom( extent.center().toQPointF(), scale );
  QVERIFY( kit.units() == QString( "m" ) );
  QVERIFY( kit.distance() == 200 );
}
