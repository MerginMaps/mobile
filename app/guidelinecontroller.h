/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GUIDELINECONTROLLER_H
#define GUIDELINECONTROLLER_H

#include <QObject>
#include <qglobal.h>

#include "qgsgeometry.h"
#include "inputmapsettings.h"

#include "maptools/recordingmaptool.h"


class GuidelineController : public QObject
{
    Q_OBJECT

    // input properties (geometry of real feature and curent crosshair position in map CRS)
    Q_PROPERTY( QgsGeometry realGeometry READ realGeometry WRITE setRealGeometry NOTIFY realGeometryChanged )
    Q_PROPERTY( QPointF crosshairPosition READ crosshairPosition WRITE setCrosshairPosition NOTIFY crosshairPositionChanged )
    Q_PROPERTY( InputMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )

    Q_PROPERTY( Vertex activeVertex READ activeVertex WRITE setActiveVertex NOTIFY activeVertexChanged )
    Q_PROPERTY( RecordingMapTool::InsertPolicy insertPolicy READ insertPolicy WRITE setInsertPolicy NOTIFY insertPolicyChanged )

    Q_PROPERTY( int activePart READ activePart WRITE setActivePart NOTIFY activePartChanged )
    Q_PROPERTY( int activeRing READ activeRing WRITE setActiveRing NOTIFY activeRingChanged )

    Q_PROPERTY( bool allowed READ allowed WRITE setAllowed NOTIFY allowedChanged )

    // output properties (real geometry + crosshair position ) in map CRS
    Q_PROPERTY( QgsGeometry guidelineGeometry READ guidelineGeometry WRITE setGuidelineGeometry NOTIFY guidelineGeometryChanged )

  public:
    explicit GuidelineController( QObject *parent = nullptr );

    const QgsGeometry &guidelineGeometry() const;
    void setGuidelineGeometry( const QgsGeometry &newGuidelineGeometry );

    QPointF crosshairPosition() const;
    void setCrosshairPosition( QPointF newCrosshairPosition );

    const QgsGeometry &realGeometry() const;
    void setRealGeometry( const QgsGeometry &newRealGeometry );

    InputMapSettings *mapSettings() const;
    void setMapSettings( InputMapSettings *newMapSettings );

    const Vertex &activeVertex() const;
    void setActiveVertex( const Vertex &newActiveVertex );

    bool allowed() const;
    void setAllowed( bool newAllowed );

    const RecordingMapTool::InsertPolicy &insertPolicy() const;
    void setInsertPolicy( const RecordingMapTool::InsertPolicy &insertPolicy );

    int activePart() const;
    void setActivePart( int newActivePart );

    int activeRing() const;
    void setActiveRing( int newActiveRing );

  signals:

    void guidelineGeometryChanged( const QgsGeometry &guidelineGeometry );
    void crosshairPositionChanged( QPointF crosshairPosition );

    void realGeometryChanged( const QgsGeometry &realGeometry );

    void mapSettingsChanged( InputMapSettings *mapSettings );

    void activeVertexChanged( const Vertex &activeVertex );

    void allowedChanged( bool allowed );

    void activePartChanged( int activePart );

    void insertPolicyChanged( const RecordingMapTool::InsertPolicy &insertPolicy );

    void activeRingChanged( int activeRing );

  private slots:
    void buildGuideline();

  private:

    QgsGeometry mGuidelineGeometry;
    QPointF mCrosshairPosition;
    QgsGeometry mRealGeometry;
    InputMapSettings *mMapSettings = nullptr; // not owned
    Vertex mActiveVertex;
    bool mAllowed = true;
    RecordingMapTool::InsertPolicy mInsertPolicy;
    int mActivePart = 0;
    int mActiveRing = 0;
};

#endif // GUIDELINECONTROLLER_H
