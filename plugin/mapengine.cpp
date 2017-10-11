#include "mapengine.h"

#include <QtDebug>
#include <QPainter>

#include <qgsapplication.h>
#include <qgscoordinatetransform.h>
#include <qgsrasterlayer.h>
#include <qgsproject.h>
#include <qgsmapsettings.h>
#include <qgsdistancearea.h>
#include <qgsfeatureiterator.h>
#include <qgsvectordataprovider.h>
#include <qgsvectorlayer.h>
#include <qgspointxy.h>
#include <qgsunittypes.h>

#include "mapview.h"

//#include "gisutils.h"



MapEngine::MapEngine(QObject *parent)
  : QObject(parent)
  , mView(0)
  , mWgs2map(0)
  , mDa(new QgsDistanceArea)
{

  mDa->setEllipsoid("WGS84");

  connect(this, SIGNAL(mapSettingsChanged()), this, SLOT(updateScaleBar()));
}

MapEngine::~MapEngine()
{
  delete mWgs2map;
  delete mDa;
}

void MapEngine::setView(MapView* v)
{
  if (mView)
    disconnect(mView, SIGNAL(changed()), this, SLOT(mapViewChanged()));

  mView = v;

  if (mView)
    connect(mView, SIGNAL(changed()), this, SLOT(mapViewChanged()));

  emit mapSettingsChanged();
}

void MapEngine::mapViewChanged()
{
  mMapSettings.setOutputSize(mView->size());
  mMapSettings.setExtent(qrect2qgsrect(mView->toExtent()));
  emit mapSettingsChanged();
}

void MapEngine::setDestinationCRS(const QString& crs)
{
  QgsCoordinateReferenceSystem c(crs);
  if (!c.isValid())
  {
    qWarning() << "MapEngine: destination CRS is not valid: " << crs;
    return;
  }

  // try to reproject current extent to the new one
  QgsRectangle rect;
  if ( !mMapSettings.visibleExtent().isEmpty() )
  {
    QgsCoordinateTransform transform( mMapSettings.destinationCrs(), c );
    try
    {
      rect = transform.transformBoundingBox( mMapSettings.visibleExtent() );
    }
    catch ( QgsCsException &e )
    {
      qWarning() << "MapEngine: transform error: " << e.what();
    }
  }
  if ( !rect.isEmpty() )
  {
    mMapSettings.setExtent(rect);
  }

  mMapSettings.setDestinationCrs(c);

  // for conversions from WGS 84
  delete mWgs2map;
  mWgs2map = new QgsCoordinateTransform(QgsCoordinateReferenceSystem("EPSG:4326"), mMapSettings.destinationCrs());

  mDa->setSourceCrs(mMapSettings.destinationCrs());

  emit mapSettingsChanged();
}

QString MapEngine::destinationCRS() const
{
  return mMapSettings.destinationCrs().authid();
}


QPointF MapEngine::wgs84ToMap(const QPointF& wgs84Point)
{
  if (!mWgs2map)
    return QPointF();
  QgsPointXY mapPoint = mWgs2map->transform(wgs84Point.x(), wgs84Point.y());
  return mapPoint.toQPointF();
}

double MapEngine::scale2mupp(double scale)
{
  double scaleFactor = scale / mMapSettings.scale();
  double mupp = mMapSettings.mapUnitsPerPixel() * scaleFactor;
  return mupp;
}


double MapEngine::metersPerPixel() const
{
  // assuming map unit = 1px (projected CRS with meters as map units)
  if (mMapSettings.destinationCrs().mapUnits() == QgsUnitTypes::DistanceMeters)
    return mMapSettings.mapUnitsPerPixel();
  else
  {
    return screenUnitsToMeters(1);
  }
}

void MapEngine::setLayers(const QList<QgsMapLayer *> layers)
{
  foreach (QgsMapLayer* ml, mMapSettings.layers())
  {
      disconnect(ml, SIGNAL(repaintRequested()), this, SLOT(onRepaintRequested()));
  }

  mMapSettings.setLayers(layers);
  foreach (QgsMapLayer* ml, layers)
  {
      connect(ml, SIGNAL(repaintRequested()), this, SLOT(onRepaintRequested()));
  }
  emit mapSettingsChanged();
}


QRectF MapEngine::layerExtent(const QgsMapLayer* ml) const
{
  return ml ? qgsrect2qrect(mMapSettings.layerExtentToOutputExtent(ml, ml->extent())) : QRectF();
}

QRectF MapEngine::fullExtent() const
{
  return qgsrect2qrect(mMapSettings.fullExtent());
}


static QList<QgsAttributes> _identifyVector(QgsVectorLayer* vlayer, const QPointF& point, MapEngine* engine)
{
  double rad = engine->mapSettings().mapUnitsPerPixel() * 10;
  QgsRectangle rect(point.x() - rad, point.y() - rad, point.x() + rad, point.y() + rad);

  if (engine->mapSettings().destinationCrs() != vlayer->dataProvider()->crs())
    rect = engine->mapSettings().layerTransform(vlayer).transform(rect, QgsCoordinateTransform::ReverseTransform);

  QList<QgsAttributes> lst;

  QgsFeatureRequest request;
  request.setFilterRect(rect);
  request.setFlags(request.flags() | QgsFeatureRequest::ExactIntersect);
  QgsFeature f;
  QgsFeatureIterator fi = vlayer->getFeatures(request);
  while (fi.nextFeature(f))
  {
    lst << f.attributes();
  }
  return lst;
}


void MapEngine::identifyPoint(const QPointF& point)
{
  //QList<QVariant> results;
  QPointF mapPoint = mView->displayToMap(point);

  QVariantMap vm;
  foreach (QgsMapLayer* layer, layers())
  {
    if (QgsVectorLayer* vlayer = qobject_cast<QgsVectorLayer*>(layer))
    {
      QList<QgsAttributes> lst = _identifyVector(vlayer, mapPoint, this);
      if (lst.count() == 0)
        continue;

      QgsFields fields = vlayer->pendingFields();
      //for (int i = 0; i < lst.count(); ++i)
      int i = 0; // take only first feature
      {
        // save all fields
        for (int j = 0; j < fields.count(); ++j)
          vm[fields[j].name()] = lst[i][j];
      }

      vm["__layer__"] = layer->name();
      break;
    }
    vm["__layer__"] = "__none__";
  }

  mIdentifyResult = vm;
  emit identifyResultChanged();
}

double MapEngine::screenUnitsToMeters(int baseLength) const
{
  // calculate the geographic distance from the central point of extent
  // to the specified number of points on the right side
  QSize s = mMapSettings.outputSize();
  QPoint pointCenter(s.width()/2, s.height()/2);
  QgsPointXY p1 = mMapSettings.mapToPixel().toMapCoordinates(pointCenter);
  QgsPointXY p2 = mMapSettings.mapToPixel().toMapCoordinates(pointCenter+QPoint(baseLength,0));
  return mDa->measureLine(p1, p2);
}


void MapEngine::updateScaleBar()
{
  int baseLength = 100;
  double dist = screenUnitsToMeters(baseLength) / 1000.0; // meters to kilometers

  // we want to show nice round distances e.g. 200 km instead of e.g. 273 km
  // so we determine which "nice" number to use and also update the scale bar
  // length accordingly. First digit will be 1, 2 or 5, the rest will be zeroes.
  int digits = floor(log10((dist))); // number of digits after first one
  double base = pow(10,digits); // e.g. for 1234 this will be 1000
  double first_digit = dist / base; // get the first digit
  int round_digit;
  if (first_digit < 2)
    round_digit = 1;
  else if (first_digit < 5)
    round_digit = 2;
  else
    round_digit = 5;
  double round_dist = round_digit * base;

  mScaleBarLength = baseLength * round_dist/dist;
  mScaleBarText = QString("%1 km").arg(round_dist);

  emit scaleBarChanged();
}

void MapEngine::onRepaintRequested()
{
  //refreshMap();
}


QRectF MapEngine::extent() const
{
  return qgsrect2qrect(mMapSettings.visibleExtent());
}

#if 0  // extent config now done with MapView
void MapEngine::setExtent(const QRectF& extent)
{
  QgsRectangle rect = qrect2qgsrect(extent);
  //rect.scale(1.1);  // a bit of extra margins
  mMapSettings.setExtent(rect);
  emit mapSettingsChanged();
}

void MapEngine::zoomToPoint(double x, double y, double scale)
{
  QgsRectangle r = mMapSettings.extent();
  double scaleFactor = scale / mMapSettings.scale();
  r.scale(scaleFactor, x, y);
  mMapSettings.setExtent(r);
  emit mapSettingsChanged();
}

void MapEngine::zoomIn()
{
  QgsRectangle r = mMapSettings.extent();
  r.scale(0.8);
  mMapSettings.setExtent(r);
  emit mapSettingsChanged();
}

void MapEngine::zoomOut()
{
  QgsRectangle r = mMapSettings.extent();
  r.scale(1.2);
  mMapSettings.setExtent(r);
  emit mapSettingsChanged();
}

void MapEngine::move(double x0, double y0, double x1, double y1)
{
  QgsPoint pt0 = mMapSettings.mapToPixel().toMapCoordinates(x0, y0);
  QgsPoint pt1 = mMapSettings.mapToPixel().toMapCoordinates(x1, y1);
  double dx = pt1.x() - pt0.x(), dy = pt1.y() - pt0.y();
  QgsPoint center = mMapSettings.extent().center();
  double w = mMapSettings.extent().width();
  double h = mMapSettings.extent().height();
  QgsPoint c(center.x() - dx, center.y() - dy);
  mMapSettings.setExtent(QgsRectangle(c.x() - w/2, c.y() - h/2,
                                      c.x() + w/2, c.y() + h/2));
  emit mapSettingsChanged();
}

void MapEngine::scale(double s)
{
  QgsRectangle r = mMapSettings.visibleExtent();
  r.scale(1/s);
  mMapSettings.setExtent(r);
  emit mapSettingsChanged();
}
#endif
