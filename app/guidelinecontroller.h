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
#include "qgsquickmapsettings.h"

#include "maptools/recordingmaptool.h"


class GuidelineController : public QObject
{
    Q_OBJECT

    // input properties (geometry of real feature and curent crosshair position in map CRS)
    Q_PROPERTY( QgsGeometry realGeometry READ realGeometry WRITE setRealGeometry NOTIFY realGeometryChanged )
    Q_PROPERTY( QPointF crosshairPosition READ crosshairPosition WRITE setCrosshairPosition NOTIFY crosshairPositionChanged )
    Q_PROPERTY( QgsQuickMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )

    Q_PROPERTY( Vertex activeVertex READ activeVertex WRITE setActiveVertex NOTIFY activeVertexChanged )
    Q_PROPERTY( RecordingMapTool::NewVertexOrder newVertexOrder READ newVertexOrder WRITE setNewVertexOrder NOTIFY newVertexOrderChanged )

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

    QgsQuickMapSettings *mapSettings() const;
    void setMapSettings( QgsQuickMapSettings *newMapSettings );

    const Vertex &activeVertex() const;
    void setActiveVertex( const Vertex &newActiveVertex );

    bool allowed() const;
    void setAllowed( bool newAllowed );

    const RecordingMapTool::NewVertexOrder &newVertexOrder() const;
    void setNewVertexOrder( const RecordingMapTool::NewVertexOrder &newNewVertexOrder );

  signals:

    void guidelineGeometryChanged( const QgsGeometry &guidelineGeometry );
    void crosshairPositionChanged( QPointF crosshairPosition );

    void realGeometryChanged( const QgsGeometry &realGeometry );

    void mapSettingsChanged( QgsQuickMapSettings *mapSettings );

    void activeVertexChanged( const Vertex &activeVertex );

    void allowedChanged( bool allowed );

    void newVertexOrderChanged( const RecordingMapTool::NewVertexOrder &newVertexOrder );

  private slots:
    void buildGuideline();

  private:

    QgsGeometry mGuidelineGeometry;
    QPointF mCrosshairPosition;
    QgsGeometry mRealGeometry;
    QgsQuickMapSettings *mMapSettings = nullptr; // not owned
    Vertex mActiveVertex;
    bool mAllowed;
    RecordingMapTool::NewVertexOrder mNewVertexOrder;
};

#endif // GUIDELINECONTROLLER_H
