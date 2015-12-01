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


class MapView : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QPointF center READ center WRITE setCenter NOTIFY changed)
  Q_PROPERTY(double mupp READ mupp WRITE setMupp NOTIFY changed)
  Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY changed)
  Q_PROPERTY(bool valid READ isValid NOTIFY changed)
  Q_PROPERTY(MapView* parentView READ parentView WRITE setParentView NOTIFY changed)
  Q_PROPERTY(double dxToParent READ dxToParent NOTIFY parentChanged)
  Q_PROPERTY(double dyToParent READ dyToParent NOTIFY parentChanged)
  Q_PROPERTY(double scaleToParent READ scaleToParent NOTIFY parentChanged)
public:
  explicit MapView(QObject* parent = 0)
    : QObject(parent), mCenter(), mMupp(0), mSize(), mParentView(0)
    , mDxToParent(0), mDyToParent(0), mScaleToParent(0) {}

  void setCenter(const QPointF& center) { mCenter = center; updateParams(); }
  void setMupp(double mupp) { mMupp = mupp; updateParams(); }
  void setSize(const QSize& size) { mSize = size; updateParams(); }

  QPointF center() const { return mCenter; }
  double mupp() const { return mMupp; }
  QSize size() const { return QSize(xform.mapWidth(), xform.mapHeight()); }
  bool isValid() const { return mSize.width() > 0 && mSize.height() > 0 && mMupp > 0; }

  Q_INVOKABLE void fromExtent(const QRectF& rect)
  {
    if (!mSize.isValid())
    {
      qDebug("null size - fromExtent() does nothing!");
      return;
    }

    mCenter = rect.center();
    mMupp = rect.width() / mSize.width(); // TODO: also consider height?
    updateParams();
  }

  Q_INVOKABLE QRectF toExtent()
  {
    double w = mSize.width() * mMupp;
    double h = mSize.height() * mMupp;
    return QRectF(mCenter.x() - w/2, mCenter.y() - h/2, w, h);
  }

  Q_INVOKABLE QPointF mapToDisplay(const QPointF& point) {
    return xform.transform(point.x(), point.y()).toQPointF();
  }

  Q_INVOKABLE QPointF displayToMap(const QPointF& point)
  {
    QgsPoint p = xform.toMapCoordinatesF(point.x(), point.y());
    return QPointF(p.x(), p.y());
  }

  MapView* parentView() const { return mParentView; }
  void setParentView(MapView* mv) {
    mParentView = mv;
    if (mParentView)
      connect(mParentView, SIGNAL(changed()), this, SLOT(updateTransformToParent()));
    emit changed();
  }

  Q_INVOKABLE void copyFrom(MapView* mv) {
    mCenter = mv->center();
    mMupp = mv->mupp();
    mSize = mv->size();
    updateParams();
  }

  double dxToParent() const { return mDxToParent; }
  double dyToParent() const { return mDyToParent; }
  double scaleToParent() const { return mScaleToParent; }

signals:
  void changed();
  void parentChanged();

protected slots:
  void updateTransformToParent() {
    if (!mParentView || mParentView->mupp() == 0 || mMupp == 0)
      return;

    QTransform t_c = mParentView->xform.transform();
    QTransform t_m = xform.transform();
    QTransform t = t_m.inverted() * t_c;
    qDebug() << "xxx " << t.dx() << " " << t.dy() << " | " << t.m11();
    mDxToParent = t.dx();
    mDyToParent = t.dy();
    mScaleToParent = t.m11();
    emit parentChanged();
  }

protected:
  void updateParams()
  {
    xform.setParameters(mMupp, mCenter.x(), mCenter.y(), mSize.width(), mSize.height(), 0);
    emit changed();
    updateTransformToParent();
  }

  QPointF mCenter;
  double mMupp;
  QSize mSize;
  QgsMapToPixel xform;
  MapView* mParentView;
  double mDxToParent, mDyToParent, mScaleToParent;
};


class MapEngine : public QObject
{
  Q_OBJECT
  Q_PROPERTY(MapView* view READ view WRITE setView NOTIFY mapSettingsChanged)
  Q_PROPERTY(QString destinationCRS READ destinationCRS WRITE setDestinationCRS NOTIFY mapSettingsChanged)
  Q_PROPERTY(QString scaleBarText READ scaleBarText NOTIFY scaleBarChanged)
  Q_PROPERTY(int scaleBarLength READ scaleBarLength NOTIFY scaleBarChanged)
  Q_PROPERTY(double metersPerPixel READ metersPerPixel NOTIFY mapSettingsChanged)
  Q_PROPERTY(QStringList layers READ layers WRITE setLayers NOTIFY mapSettingsChanged)
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

  //! convert from lat/lon coordinates (wgs84) to coords of the rendered map image
  Q_INVOKABLE QPointF convertWgs84ToImageCoords(const QPointF& wgs84Point);

  QString scaleBarText() const { return mScaleBarText; }
  int scaleBarLength() const { return mScaleBarLength; }

  double metersPerPixel() const;

  QStringList layers() const { return mMapSettings.layers(); }
  void setLayers(const QStringList& layers);

  QRectF extent() const;

  Q_INVOKABLE QRectF layerExtent(const QString& layerId) const;

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

  int mScaleBarLength; // in pixels
  QString mScaleBarText;

  QgsMapRendererCache* mCache;

  QVariant mIdentifyResult;
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

  Q_INVOKABLE void refreshMap(MapView* mv);

  QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData);

signals:
  void mapEngineChanged();
  void mapImageChanged();

protected slots:
  void refreshMapDelayed();
  void jobFinished();

protected:
  MapEngine* mEngine;
  bool mNewImg;

  QgsMapSettings mMapSettings;

  bool mRefreshRequested;
  bool mJobCancelled;
  QgsMapRendererParallelJob* mJob;
  QImage mMapImage;

  QgsMapRendererCache* mCache;
};


#endif // MAPENGINE_H
