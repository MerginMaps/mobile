/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DIGITIZINGCONTROLLER_H
#define DIGITIZINGCONTROLLER_H

#include <QObject>

#include "qgscoordinatetransform.h"
#include "qgsfeature.h"
#include "qgsgeometry.h"
#include "qgsquickmapsettings.h"
#include "qgsquickpositionkit.h"
#include "qgsquickfeaturelayerpair.h"

class DigitizingController : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QgsQuickFeatureLayerPair featureLayerPair READ featureLayerPair WRITE setFeatureLayerPair NOTIFY layerChanged )
    Q_PROPERTY( QgsVectorLayer *layer READ layer WRITE setLayer NOTIFY layerChanged )
    Q_PROPERTY( bool recording READ isRecording NOTIFY recordingChanged )
    Q_PROPERTY( bool manualRecording READ manualRecording WRITE setManualRecording NOTIFY manualRecordingChanged )
    Q_PROPERTY( int lineRecordingInterval READ lineRecordingInterval WRITE setLineRecordingInterval NOTIFY lineRecordingIntervalChanged )
    Q_PROPERTY( QgsQuickPositionKit *positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged )
    Q_PROPERTY( QgsQuickMapSettings *mapSettings MEMBER mMapSettings NOTIFY mapSettingsChanged )
    //! If True, recorded point is from GPS and contains z-coord
    Q_PROPERTY( bool useGpsPoint MEMBER mUseGpsPoint NOTIFY useGpsPointChanged )

  public:
    explicit DigitizingController( QObject *parent = nullptr );

    QgsQuickPositionKit *positionKit() const { return mPositionKit; }
    void setPositionKit( QgsQuickPositionKit *kit );

    QgsVectorLayer *layer() const;
    void setLayer( QgsVectorLayer *layer );
    QgsQuickFeatureLayerPair featureLayerPair() const;
    void setFeatureLayerPair( QgsQuickFeatureLayerPair pair );


    Q_INVOKABLE bool hasLineGeometry( QgsVectorLayer *layer ) const;
    Q_INVOKABLE bool hasPolygonGeometry( QgsVectorLayer *layer ) const;
    Q_INVOKABLE bool hasPointGeometry( QgsVectorLayer *layer ) const;
    Q_INVOKABLE bool isPairValid( QgsQuickFeatureLayerPair pair ) const;

    //! Creates a new QgsFeature with point geometry from the given point with map coordinates.
    Q_INVOKABLE QgsQuickFeatureLayerPair pointFeatureFromPoint( const QgsPoint &point, bool isGpsPoint );
    //! Creates a new QgsFeature with line/polygon geometry from the points stored since the start of recording
    Q_INVOKABLE QgsQuickFeatureLayerPair lineOrPolygonFeature();
    //! Creates a new QgsFeature without geometry
    Q_INVOKABLE QgsQuickFeatureLayerPair featureWithoutGeometry();
    //! Returns (point geom) featurePair coords in map coordinates.
    Q_INVOKABLE QgsPoint pointFeatureMapCoordinates( QgsQuickFeatureLayerPair pair );
    //! Changes point geometry of given pair according given point.
    Q_INVOKABLE QgsQuickFeatureLayerPair changePointGeometry( QgsQuickFeatureLayerPair pair, QgsPoint point, bool isGpsPoint );

    Q_INVOKABLE void addRecordPoint( const QgsPoint &point, bool isGpsPoint );
    Q_INVOKABLE void removeLastPoint();

    Q_INVOKABLE void startRecording();
    Q_INVOKABLE void stopRecording();
    bool isRecording() const { return mRecording; }

    std::unique_ptr<QgsPoint> getLayerPoint( const QgsPoint &point, bool isGpsPoint );
    QgsGeometry getPointGeometry( const QgsPoint &point, bool isGpsPoint );
    QgsQuickFeatureLayerPair createFeatureLayerPair( const QgsGeometry &geometry );

    int lineRecordingInterval() const;
    void setLineRecordingInterval( int lineRecordingInterval );

    bool manualRecording() const;
    void setManualRecording( bool manualRecording );

    bool useGpsPoint() const;
    void setUseGpsPoint( bool useGpsPoint );

  signals:
    void layerChanged();
    void recordingChanged();
    void manualRecordingChanged();
    void positionKitChanged();
    void recordingFeatureModelChanged();
    void mapSettingsChanged();
    void lineRecordingIntervalChanged();
    void useGpsPointChanged();

  private slots:
    void onPositionChanged();

  private:
    void fixZ( QgsPoint &point ) const; // add/remove Z coordinate based on layer wkb type
    QgsCoordinateTransform transformer() const;
    QgsQuickFeatureLayerPair lineFeature();
    QgsQuickFeatureLayerPair polygonFeature();
    bool hasEnoughPoints() const;

    bool mRecording = false;
    //! Flag if a point is added to mRecordedPoints by user interaction (true) or onPositionChanged (false)
    //! Used only for polyline and polygon features.
    bool mManualRecording = true;
    QgsQuickPositionKit *mPositionKit = nullptr;
    QVector<QgsPoint> mRecordedPoints;  //!< for recording of linestrings, point's coord in layer CRS
    QgsQuickFeatureLayerPair mFeatureLayerPair; //!< to be used for highlight of feature being recorded
    QgsQuickMapSettings *mMapSettings = nullptr;
    int mLineRecordingInterval = 3; // in seconds
    QDateTime mLastTimeRecorded;
    bool mUseGpsPoint = false;
};

#endif // DIGITIZINGCONTROLLER_H
