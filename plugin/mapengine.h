#ifndef MAPENGINE_H
#define MAPENGINE_H

#include <QObject>
#include <QSize>

#include "qgsmapsettings.h"

class QTimer;

class QgsRasterLayer;
class QgsVectorLayer;

class MapView;


inline QgsRectangle qrect2qgsrect(const QRectF& rect)
{
  return QgsRectangle(rect.left(), rect.top(), rect.right(), rect.bottom());
}

inline QRectF qgsrect2qrect(const QgsRectangle& rect)
{
  return QRectF(QPointF(rect.xMinimum(), rect.yMinimum()), QPointF(rect.xMaximum(), rect.yMaximum()));
}


class MapEngine : public QObject
{
  Q_OBJECT
  Q_PROPERTY(MapView* view READ view WRITE setView NOTIFY mapSettingsChanged)
  Q_PROPERTY(QString destinationCRS READ destinationCRS WRITE setDestinationCRS NOTIFY mapSettingsChanged)
  Q_PROPERTY(QString scaleBarText READ scaleBarText NOTIFY scaleBarChanged)
  Q_PROPERTY(int scaleBarLength READ scaleBarLength NOTIFY scaleBarChanged)
  Q_PROPERTY(double metersPerPixel READ metersPerPixel NOTIFY mapSettingsChanged)
  Q_PROPERTY(QList<QgsMapLayer*> layers READ layers WRITE setLayers NOTIFY mapSettingsChanged)
  Q_PROPERTY(QRectF extent READ extent NOTIFY mapSettingsChanged)
  Q_PROPERTY(QVariant identifyResult READ identifyResult NOTIFY identifyResultChanged)
public:
  explicit MapEngine(QObject *parent = 0);
  ~MapEngine();

  MapView* view() const { return mView; }
  void setView(MapView* v);

  void setDestinationCRS(const QString& crs);
  QString destinationCRS() const;

  const QgsMapSettings& mapSettings() const { return mMapSettings; }

  //! convert from lat/lon coordinates (wgs84) to map coords
  Q_INVOKABLE QPointF wgs84ToMap(const QPointF& wgs84Point);
  //! find out right mupp for given scale denominator and current settings
  Q_INVOKABLE double scale2mupp(double scale);

  QString scaleBarText() const { return mScaleBarText; }
  int scaleBarLength() const { return mScaleBarLength; }

  double metersPerPixel() const;

  QList<QgsMapLayer*> layers() const { return mMapSettings.layers(); }
  void setLayers(const QList<QgsMapLayer*> layers);

  QRectF extent() const;

  Q_INVOKABLE QRectF layerExtent(const QgsMapLayer *ml) const;

  Q_INVOKABLE QRectF fullExtent() const;

  Q_INVOKABLE void identifyPoint(const QPointF& point);
  QVariant identifyResult() const { return mIdentifyResult; }

signals:
  void mapSettingsChanged();
  void scaleBarChanged();
  void identifyResultChanged();

public slots:
  void mapViewChanged();
  void updateScaleBar();
  void onRepaintRequested();

#if 0  // extent config now done with MapView
  void setExtent(const QRectF& extent);
  Q_INVOKABLE void zoomToPoint(double x, double y, double scale);
  Q_INVOKABLE void zoomIn();
  Q_INVOKABLE void zoomOut();
  Q_INVOKABLE void move(double x0, double y0, double x1, double y1);
  Q_INVOKABLE void scale(double s);
#endif

protected:
  double screenUnitsToMeters(int baseLength) const;

private:

  MapView* mView;
  QgsMapSettings mMapSettings;
  QgsCoordinateTransform* mWgs2map;
  QgsDistanceArea* mDa;

  int mScaleBarLength; // in pixels
  QString mScaleBarText;

  QVariant mIdentifyResult;
};




#endif // MAPENGINE_H
