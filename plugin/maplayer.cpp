#include "maplayer.h"

#include <qgsrasterlayer.h>
#include <qgsproject.h>

MapLayer::MapLayer()
  : mRL(0)
{
}

QString MapLayer::layerId() const
{
  return mRL ? mRL->id() : QString();
}

void MapLayer::setSource(const QString& src)
{
  mSource = src;
  emit sourceChanged();

  load();
}

void MapLayer::setProvider(const QString& prv)
{
  mProvider = prv;
  emit providerChanged();

  load();
}

void MapLayer::load()
{
  if (mSource.isEmpty() || mProvider.isEmpty())
    return;

  mRL = new QgsRasterLayer(mSource);
  Q_ASSERT(mRL->isValid());
  QgsProject::instance()->addMapLayer(mRL);
  qDebug("LOADED!");
  emit layerLoaded();
}

