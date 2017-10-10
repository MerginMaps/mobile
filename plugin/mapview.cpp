#include "mapview.h"
#include <qgspointxy.h>
#include <QtDebug>

MapView::MapView(QObject* parent)
  : QObject(parent), mCenter(), mMupp(0), mSize(), mParentView(0)
  , mDxToParent(0), mDyToParent(0), mScaleToParent(0)
{
}

void MapView::setCenter(const QPointF& center)
{
  mCenter = center;
  updateParams();
}

void MapView::setMupp(double mupp)
{
  mMupp = mupp;
  updateParams();
}

void MapView::setSize(const QSize& size)
{
  mSize = size;
  updateParams();
}

void MapView::fromExtent(const QRectF& rect)
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

QRectF MapView::toExtent()
{
  double w = mSize.width() * mMupp;
  double h = mSize.height() * mMupp;
  return QRectF(mCenter.x() - w/2, mCenter.y() - h/2, w, h);
}

QPointF MapView::mapToDisplay(const QPointF& point)
{
  return xform.transform(point.x(), point.y()).toQPointF();
}

QPointF MapView::displayToMap(const QPointF& point)
{
  QgsPointXY p = xform.toMapCoordinatesF(point.x(), point.y());
  return QPointF(p.x(), p.y());
}

void MapView::setParentView(MapView* mv)
{
  mParentView = mv;
  if (mParentView)
    connect(mParentView, SIGNAL(changed()), this, SLOT(updateTransformToParent()));
  emit changed();
}

void MapView::copyFrom(MapView* mv)
{
  mCenter = mv->center();
  mMupp = mv->mupp();
  mSize = mv->size();
  updateParams();
}

void MapView::updateTransformToParent()
{
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

void MapView::updateParams()
{
  xform.setParameters(mMupp, mCenter.x(), mCenter.y(), mSize.width(), mSize.height(), 0);
  emit changed();
  updateTransformToParent();
}
