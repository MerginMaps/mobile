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

    // When editing geometry - set this as the geometry to start with
    Q_PROPERTY( QgsGeometry initialGeometry READ initialGeometry WRITE setInitialGeometry NOTIFY initialGeometryChanged )

  public:

    enum RecordingType
    {
      StreamMode = 0,
      Manual
    };
    Q_ENUM( RecordingType );

    explicit RecordingMapTool( QObject *parent = nullptr );
    virtual ~RecordingMapTool();

    //! Adds point to the end of the recorded geometry; updates recordedGeometry afterwards
    Q_INVOKABLE void addPoint( const QPointF &point );

    /**
     *  Removes last point from recorded geometry if there is at least one point
     *  Updates recordedGeometry afterwards
     */
    Q_INVOKABLE void removePoint();

    //! Returns true if the captured geometry has enought points for the specified layer
    Q_INVOKABLE bool hasValidGeometry() const;

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

  signals:
    void layerChanged( QgsVectorLayer *layer );
    void centeredToGPSChanged( bool centeredToGPS );
    void positionKitChanged( PositionKit *positionKit );
    void recordedGeometryChanged( const QgsGeometry &recordedGeometry );
    void recordingIntervalChanged( int lineRecordingInterval );
    void recordingTypeChanged( const RecordingMapTool::RecordingType &recordingType );

    void initialGeometryChanged( const QgsGeometry &initialGeometry );

  public slots:
    void onPositionChanged();

  protected:
    //! Takes the captured points and builds a QgsGeometry from it, based on layer wkb type
    void rebuildGeometry();

    //! Unifies Z coordinate of the point with current layer - drops / adds it
    void fixZ( QgsPoint &point ) const;

  private:
    QVector<QgsPoint> mPoints;
    QgsGeometry mRecordedGeometry;
    QgsGeometry mInitialGeometry;

    bool mCenteredToGPS;
    int mRecordingInterval;  // in seconds for the StreamingMode
    RecordingType mRecordingType = Manual;

    QDateTime mLastTimeRecorded;

    QgsVectorLayer *mLayer = nullptr; // not owned
    PositionKit *mPositionKit = nullptr; // not owned
};

#endif // RECORDINGMAPTOOL_H
