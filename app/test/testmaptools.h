/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTMAPTOOLS_H
#define TESTMAPTOOLS_H

#include <QObject>
#include <qglobal.h>

#include "positionkit.h"
#include "appsettings.h"

class TestMapTools : public QObject
{
    Q_OBJECT
  public:
    TestMapTools( PositionKit *pk, AppSettings *as );
  private slots:
    void init();
    void cleanup();

    void testSnapping();

    void testSplitting();
    void testCanCommitSplit();

    void testRecording();
    void testMeasuring();

    void testExistingVertices();
    void testMidSegmentVertices();
    void testHandles();
    void testLookForVertex();

    void testAddVertexPointLayer();
    void testAddVertexMultiPointLayer();
    void testAddVertexLineLayer();
    void testAddVertexMultiLineLayer();
    void testAddVertexPolygonLayer();
    void testAddVertexMultiPolygonLayer();
    void testUpdateVertex();
    void testRemoveVertex();
    void testVerticesStructure();

    void testZMRecording();

    void testAntennaHeight();
    void testSmallTracking();

    void testAvoidIntersections();

  private:
    PositionKit *mPositionKit;
    AppSettings *mAppSettings;
};

#endif // TESTMAPTOOLS_H
