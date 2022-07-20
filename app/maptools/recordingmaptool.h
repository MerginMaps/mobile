/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RECORDINGMAPTOOL_H
#define RECORDINGMAPTOOL_H

#include "abstractmaptool.h"

#include <QObject>
#include <qglobal.h>

#include "qgsgeometry.h"

class PositionKit;
class VariablesManager;
class QgsVectorLayer;

class RecordingMapTool : public AbstractMapTool
{
    Q_OBJECT

    Q_PROPERTY( bool centeredToGPS READ centeredToGPS WRITE setCenteredToGPS NOTIFY centeredToGPSChanged )
    Q_PROPERTY( RecordingType recordingType READ recordingType WRITE setRecordingType NOTIFY recordingTypeChanged )
    Q_PROPERTY( int recordingInterval READ recordingInterval WRITE setRecordingInterval NOTIFY recordingIntervalChanged )

    Q_PROPERTY( QgsVectorLayer *layer READ layer WRITE setLayer NOTIFY layerChanged )
    Q_PROPERTY( PositionKit *positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged )

    Q_PROPERTY( QgsGeometry recordedGeometry READ recordedGeometry WRITE setRecordedGeometry NOTIFY recordedGeometryChanged )
    Q_PROPERTY( QgsGeometry existingVertices READ existingVertices WRITE setExistingVertices NOTIFY existingVerticesChanged )
    Q_PROPERTY( QgsGeometry midPoints READ midPoints WRITE setMidPoints NOTIFY midPointsChanged )
    Q_PROPERTY( QgsGeometry handles READ handles WRITE setHandles NOTIFY handlesChanged )

    // When editing geometry - set this as the geometry to start with
    Q_PROPERTY( QgsGeometry initialGeometry READ initialGeometry WRITE setInitialGeometry NOTIFY initialGeometryChanged )

    Q_PROPERTY( QString state READ state WRITE setState NOTIFY stateChanged )


  public:

    enum RecordingType
    {
      StreamMode = 0,
      Manual
    };
    Q_ENUM( RecordingType );

    explicit RecordingMapTool( QObject *parent = nullptr );
    virtual ~RecordingMapTool();

    /**
     * Adds point to the end of the recorded geometry; updates recordedGeometry afterwards
     * Passed point needs to be in active vector layer CRS
     */
    Q_INVOKABLE void addPoint( const QgsPoint &point );

    /**
     *  Removes last point from recorded geometry if there is at least one point
     *  Updates recordedGeometry afterwards
     */
    Q_INVOKABLE void removePoint();

    //! Returns true if the captured geometry has enought points for the specified layer
    Q_INVOKABLE bool hasValidGeometry() const;

    /**
     * Create a multi-point geometry that can be used to highlight vertices of a feature
     */
    Q_INVOKABLE QgsGeometry extractGeometryVertices( const QgsGeometry &geometry );

    /**
     * Create a multi-point geometry that can be used to highlight "virtual" nodes representing
     * the middle of segments. For lines also creates "virtual" nodes at the beginning and end
     * of the line.
     */
    Q_INVOKABLE QgsGeometry extractMidSegmentVertices( const QgsGeometry &geometry );

    /**
     * Create "handles" at the beginnig and end of the line geometry. Returns null geometry for
     * other geometry types.
     */
    Q_INVOKABLE QgsGeometry createHandles( const QgsGeometry &geometry );

    // Getters / setters
    bool centeredToGPS() const;
    void setCenteredToGPS( bool newCenteredToGPS );

    const RecordingType &recordingType() const;
    void setRecordingType( const RecordingType &newRecordingType );

    int recordingInterval() const;
    void setRecordingInterval( int newRecordingInterval );

    PositionKit *positionKit() const;
    void setPositionKit( PositionKit *newPositionKit );

    QgsVectorLayer *layer() const;
    void setLayer( QgsVectorLayer *newLayer );

    const QgsGeometry &recordedGeometry() const;
    void setRecordedGeometry( const QgsGeometry &newRecordedGeometry );

    const QgsGeometry &initialGeometry() const;
    // Fills mPoints array with points from the geometry
    void setInitialGeometry( const QgsGeometry &newInitialGeometry );

    const QgsGeometry &existingVertices() const;
    void setExistingVertices( const QgsGeometry &newExistingVertices );

    const QgsGeometry &midPoints() const;
    void setMidPoints( const QgsGeometry &newMidPoints );

    const QgsGeometry &handles() const;
    void setHandles( const QgsGeometry &newHandles );

    const QString &state() const;
    void setState( const QString &newState );

  signals:
    void layerChanged( QgsVectorLayer *layer );
    void centeredToGPSChanged( bool centeredToGPS );
    void positionKitChanged( PositionKit *positionKit );
    void recordedGeometryChanged( const QgsGeometry &recordedGeometry );
    void recordingIntervalChanged( int lineRecordingInterval );
    void recordingTypeChanged( const RecordingMapTool::RecordingType &recordingType );

    void initialGeometryChanged( const QgsGeometry &initialGeometry );

    void existingVerticesChanged( const QgsGeometry &existingVertices );

    void midPointsChanged( const QgsGeometry &midPoints );

    void handlesChanged( const QgsGeometry &handles );

    void stateChanged( const QString &state );

  public slots:
    void onPositionChanged();

  protected:
    //! Takes the captured points and builds a QgsGeometry from it, based on layer wkb type
    void rebuildGeometry();

    //! Unifies Z coordinate of the point with current layer - drops / adds it
    void fixZ( QgsPoint &point ) const;

    QVector<QgsPoint> mPoints;

  private:
    QgsGeometry mRecordedGeometry;
    QgsGeometry mInitialGeometry;

    bool mCenteredToGPS;
    int mRecordingInterval;  // in seconds for the StreamingMode
    RecordingType mRecordingType = Manual;

    QDateTime mLastTimeRecorded;

    QgsVectorLayer *mLayer = nullptr; // not owned
    PositionKit *mPositionKit = nullptr; // not owned
    QgsGeometry mExistingVertices;
    QgsGeometry mMidPoints;
    QgsGeometry mHandles;

    QString mState = "view";
};

#endif // RECORDINGMAPTOOL_H
