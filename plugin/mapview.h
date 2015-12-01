#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QObject>

#include <qgsmaptopixel.h>

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
  explicit MapView(QObject* parent = 0);

  void setCenter(const QPointF& center);
  void setMupp(double mupp);
  void setSize(const QSize& size);

  QPointF center() const { return mCenter; }
  double mupp() const { return mMupp; }
  QSize size() const { return QSize(xform.mapWidth(), xform.mapHeight()); }
  bool isValid() const { return mSize.width() > 0 && mSize.height() > 0 && mMupp > 0; }

  Q_INVOKABLE void fromExtent(const QRectF& rect);

  Q_INVOKABLE QRectF toExtent();

  Q_INVOKABLE QPointF mapToDisplay(const QPointF& point);

  Q_INVOKABLE QPointF displayToMap(const QPointF& point);

  MapView* parentView() const { return mParentView; }
  void setParentView(MapView* mv);

  Q_INVOKABLE void copyFrom(MapView* mv);

  double dxToParent() const { return mDxToParent; }
  double dyToParent() const { return mDyToParent; }
  double scaleToParent() const { return mScaleToParent; }

signals:
  void changed();
  void parentChanged();

protected slots:
  void updateTransformToParent();

protected:
  void updateParams();

  QPointF mCenter;
  double mMupp;
  QSize mSize;
  QgsMapToPixel xform;
  MapView* mParentView;
  double mDxToParent, mDyToParent, mScaleToParent;
};


#endif // MAPVIEW_H
