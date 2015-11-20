#ifndef MAPENGINE_H
#define MAPENGINE_H

#include <QObject>
#include <QQuickItem>
#include <QSize>

#include "qgsmapsettings.h"

class QTimer;

class QgsMapRendererCache;
class QgsMapRendererParallelJob;
class QgsRasterLayer;
class QgsVectorLayer;

class MapEngine : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QSize imageSize READ imageSize WRITE setImageSize NOTIFY imageSizeChanged)
  Q_PROPERTY(QString destinationCRS READ destinationCRS WRITE setDestinationCRS NOTIFY mapSettingsChanged)
  Q_PROPERTY(QString scaleBarText READ scaleBarText NOTIFY scaleBarChanged)
  Q_PROPERTY(int scaleBarLength READ scaleBarLength NOTIFY scaleBarChanged)
  Q_PROPERTY(double metersPerPixel READ metersPerPixel NOTIFY mapSettingsChanged)
  Q_PROPERTY(QStringList layers READ layers WRITE setLayers NOTIFY mapSettingsChanged)
  Q_PROPERTY(QRectF extent READ extent WRITE setExtent NOTIFY mapSettingsChanged)
public:
  explicit MapEngine(QObject *parent = 0);
  ~MapEngine();

  QSize imageSize() { return mMapSettings.outputSize(); }

  void setImageSize(const QSize& s);

  void setDestinationCRS(const QString& crs);
  QString destinationCRS() const;

  const QgsMapSettings& mapSettings() const { return mMapSettings; }

  //! convert from map coordinates (projected) to coords of the rendered map image
  Q_INVOKABLE QPointF convertMapToImageCoords(const QPointF& mapPoint);
  //! convert from coords of the rendered map image to map coordinates (projected)
  Q_INVOKABLE QPointF convertImageToMapCoords(const QPointF& imagePoint);
  //! convert from lat/lon coordinates (wgs84) to coords of the rendered map image
  Q_INVOKABLE QPointF convertWgs84ToImageCoords(const QPointF& wgs84Point);

  Q_INVOKABLE void setTransparency(double value, QStringList layerIds);
  Q_INVOKABLE double transparency(QStringList layerIds);

  QString scaleBarText() const { return mScaleBarText; }
  int scaleBarLength() const { return mScaleBarLength; }

  double metersPerPixel() const;

  QStringList layers() const { return mMapSettings.layers(); }
  void setLayers(const QStringList& layers);

  QRectF extent() const;
  void setExtent(const QRectF& extent);

  Q_INVOKABLE QRectF layerExtent(const QString& layerId) const;

  QImage mapImage() const { return mMapImage; }

signals:
  void mapImageChanged();
  void imageSizeChanged();
  void mapSettingsChanged();
  void scaleBarChanged();

public slots:
  void jobFinished();
  void updateScaleBar();
  void onRepaintRequested();

  Q_INVOKABLE void zoomIn();
  Q_INVOKABLE void zoomOut();
  Q_INVOKABLE void move(double x0, double y0, double x1, double y1);
  Q_INVOKABLE void scale(double s);

protected:
  void refreshMap();
  double screenUnitsToMeters(int baseLength) const;

private:

  QgsMapSettings mMapSettings;

  QgsMapRendererParallelJob* mJob;
  QImage mMapImage;

  int mScaleBarLength; // in pixels
  QString mScaleBarText;

  QgsMapRendererCache* mCache;
};


/** Item implementation that draws map engine's last map image */
class MapImage : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(MapEngine* mapEngine READ mapEngine WRITE setMapEngine NOTIFY mapEngineChanged)

public:
  MapImage(QQuickItem* parent = 0);

  MapEngine* mapEngine() { return mEngine; }
  void setMapEngine(MapEngine* e);

  QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData);

signals:
  void mapEngineChanged();

protected slots:
  void mapImageChanged();

protected:
  MapEngine* mEngine;
  bool mNewImg;
};


#endif // MAPENGINE_H
