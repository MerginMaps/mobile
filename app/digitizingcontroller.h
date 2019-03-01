#ifndef DIGITIZINGCONTROLLER_H
#define DIGITIZINGCONTROLLER_H

#include <QObject>

#include "qgscoordinatetransform.h"
#include "qgsfeature.h"
#include "qgsgeometry.h"

#include "qgsquickattributemodel.h"
#include "qgsquickmapsettings.h"
#include "qgsquickpositionkit.h"
#include "qgsquickfeaturelayerpair.h"

class DigitizingController : public QObject
{
  Q_OBJECT

  Q_PROPERTY( QgsQuickFeatureLayerPair featureLayerPair READ featureLayerPair WRITE setFeatureLayerPair NOTIFY layerChanged)
  Q_PROPERTY(QgsVectorLayer* layer READ layer WRITE setLayer NOTIFY layerChanged)
  Q_PROPERTY(bool recording READ isRecording NOTIFY recordingChanged)
  Q_PROPERTY(int lineRecordingInterval READ lineRecordingInterval WRITE setLineRecordingInterval NOTIFY lineRecordingIntervalChanged)
  Q_PROPERTY(QgsQuickPositionKit* positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged)
  Q_PROPERTY(QgsQuickAttributeModel* recordingFeatureModel READ recordingFeatureModel NOTIFY recordingFeatureModelChanged)
  Q_PROPERTY(QgsQuickMapSettings *mapSettings MEMBER mMapSettings NOTIFY mapSettingsChanged )

public:
  explicit DigitizingController(QObject *parent = nullptr);

  QgsQuickPositionKit *positionKit() const { return mPositionKit; }
  void setPositionKit( QgsQuickPositionKit *kit );

  QgsVectorLayer *layer() const;
  void setLayer( QgsVectorLayer *layer );
  QgsQuickFeatureLayerPair featureLayerPair() const;
  void setFeatureLayerPair(QgsQuickFeatureLayerPair pair);


  Q_INVOKABLE bool hasLineGeometry( QgsVectorLayer *layer ) const;
  Q_INVOKABLE bool hasPolygonGeometry( QgsVectorLayer *layer ) const;
  Q_INVOKABLE bool hasPointGeometry( QgsVectorLayer *layer ) const;

  //! Creates a new QgsFeature with point geometry from the given point with map coordinates.
  Q_INVOKABLE QgsQuickFeatureLayerPair pointFeatureFromPoint(const QgsPoint &point);
  //! Creates a new QgsFeature with line geometry from the points stored since the start of recording
  Q_INVOKABLE QgsQuickFeatureLayerPair lineFeature();
  //! Returns (point geom) featurePair coords in map coordinates.
  Q_INVOKABLE QgsPoint pointFeatureMapCoordinates(QgsQuickFeatureLayerPair pair);
  //! Changes point geometry of given pair according given point.
  Q_INVOKABLE QgsQuickFeatureLayerPair changePointGeometry(QgsQuickFeatureLayerPair pair, QgsPoint point);

  Q_INVOKABLE void startRecording();
  Q_INVOKABLE void stopRecording();
  bool isRecording() const { return mRecording; }

  QgsQuickAttributeModel *recordingFeatureModel() const { return mRecordingModel; }

  int lineRecordingInterval() const;
  void setLineRecordingInterval(int lineRecordingInterval);

signals:
  void layerChanged();
  void recordingChanged();
  void positionKitChanged();
  void recordingFeatureModelChanged();
  void mapSettingsChanged();
  void lineRecordingIntervalChanged();

private slots:
  void onPositionChanged();

private:
  void fixZ(QgsPoint* point) const; // add/remove Z coordinate based on layer wkb type
  QgsCoordinateTransform tranformer() const;

  bool mRecording = false;
  QgsQuickPositionKit *mPositionKit = nullptr;
  QVector<QgsPoint> mRecordedPoints;  //!< for recording of linestrings
  QgsQuickAttributeModel *mRecordingModel = nullptr;  //!< to be used for highlight of feature being recorded
  QgsQuickMapSettings *mMapSettings = nullptr;
  int mLineRecordingInterval = 3; // in seconds
  QDateTime mLastTimeRecorded;
};

#endif // DIGITIZINGCONTROLLER_H
