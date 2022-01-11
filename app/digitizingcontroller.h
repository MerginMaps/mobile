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
#include "position/positionkit.h"
#include "featurelayerpair.h"
#include "variablesmanager.h"

class DigitizingController : public QObject
{
    Q_OBJECT

    Q_PROPERTY( FeatureLayerPair featureLayerPair READ featureLayerPair WRITE setFeatureLayerPair NOTIFY layerChanged )
    Q_PROPERTY( QgsVectorLayer *layer READ layer WRITE setLayer NOTIFY layerChanged )
    Q_PROPERTY( bool recording READ isRecording NOTIFY recordingChanged )
    Q_PROPERTY( bool manualRecording READ manualRecording WRITE setManualRecording NOTIFY manualRecordingChanged )
    Q_PROPERTY( int lineRecordingInterval READ lineRecordingInterval WRITE setLineRecordingInterval NOTIFY lineRecordingIntervalChanged )
    Q_PROPERTY( PositionKit *positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged )
    Q_PROPERTY( VariablesManager *variablesManager READ variablesManager WRITE setVariablesManager NOTIFY variablesManagerChanged )
    Q_PROPERTY( QgsQuickMapSettings *mapSettings MEMBER mMapSettings NOTIFY mapSettingsChanged )
    //! If True, recorded point is from GPS and contains z-coord
    Q_PROPERTY( bool useGpsPoint MEMBER mUseGpsPoint NOTIFY useGpsPointChanged )

  public:
    explicit DigitizingController( QObject *parent = nullptr );

    PositionKit *positionKit() const { return mPositionKit; }
    void setPositionKit( PositionKit *kit );

    QgsVectorLayer *layer() const;
    void setLayer( QgsVectorLayer *layer );
    FeatureLayerPair featureLayerPair() const;
    void setFeatureLayerPair( FeatureLayerPair pair );


    Q_INVOKABLE bool hasLineGeometry( QgsVectorLayer *layer ) const;
    Q_INVOKABLE bool hasPolygonGeometry( QgsVectorLayer *layer ) const;
    Q_INVOKABLE bool hasPointGeometry( QgsVectorLayer *layer ) const;
    Q_INVOKABLE bool isPairValid( FeatureLayerPair pair ) const;

    //! Creates a new QgsFeature with point geometry from the given point with map coordinates.
    Q_INVOKABLE FeatureLayerPair pointFeatureFromPoint( const QgsPoint &point, bool isGpsPoint );
    //! Creates a new QgsFeature with line/polygon geometry from the points stored since the start of recording
    Q_INVOKABLE FeatureLayerPair lineOrPolygonFeature();
    //! Creates a new QgsFeature without geometry in layer. If layer is null, it creates the feature on currently edited layer
    Q_INVOKABLE FeatureLayerPair featureWithoutGeometry( QgsVectorLayer *layer = nullptr );
    //! Returns (point geom) featurePair coords in map coordinates.
    Q_INVOKABLE QgsPoint pointFeatureMapCoordinates( FeatureLayerPair pair );
    //! Changes point geometry of given pair according given point.
    Q_INVOKABLE FeatureLayerPair changePointGeometry( FeatureLayerPair pair, QgsPoint point, bool isGpsPoint );

    Q_INVOKABLE void addRecordPoint( const QgsPoint &point, bool isGpsPoint );
    Q_INVOKABLE void removeLastPoint();

    Q_INVOKABLE void startRecording();
    Q_INVOKABLE void stopRecording();
    bool isRecording() const { return mRecording; }

    std::unique_ptr<QgsPoint> getLayerPoint( const QgsPoint &point, bool isGpsPoint );
    QgsGeometry getPointGeometry( const QgsPoint &point, bool isGpsPoint );
    FeatureLayerPair createFeatureLayerPair( const QgsGeometry &geometry, QgsVectorLayer *layer = nullptr );

    int lineRecordingInterval() const;
    void setLineRecordingInterval( int lineRecordingInterval );

    bool manualRecording() const;
    void setManualRecording( bool manualRecording );

    bool useGpsPoint() const;
    void setUseGpsPoint( bool useGpsPoint );

    VariablesManager *variablesManager() const;
    void setVariablesManager( VariablesManager *variablesManager );

  signals:
    void layerChanged();
    void recordingChanged();
    void manualRecordingChanged();
    void positionKitChanged();
    void variablesManagerChanged();
    void recordingFeatureModelChanged();
    void mapSettingsChanged();
    void lineRecordingIntervalChanged();
    void useGpsPointChanged();

  private slots:
    void onPositionChanged();

  private:
    void fixZ( QgsPoint &point ) const; // add/remove Z coordinate based on layer wkb type
    QgsCoordinateTransform transformer() const;
    bool hasEnoughPoints() const;

    bool mRecording = false;
    //! Flag if a point is added to mRecordedPoints by user interaction (true) or onPositionChanged (false)
    //! Used only for polyline and polygon features.
    bool mManualRecording = true;
    PositionKit *mPositionKit = nullptr;
    QVector<QgsPoint> mRecordedPoints;  //!< for recording of linestrings, point's coord in layer CRS
    FeatureLayerPair mFeatureLayerPair; //!< to be used for highlight of feature being recorded
    QgsQuickMapSettings *mMapSettings = nullptr;
    VariablesManager *mVariablesManager = nullptr; // not owned
    int mLineRecordingInterval = 3; // in seconds
    QDateTime mLastTimeRecorded;
    bool mUseGpsPoint = false;
};

#endif // DIGITIZINGCONTROLLER_H
