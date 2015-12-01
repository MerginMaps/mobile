#include "mapimage.h"

#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <QTimer>

#include <qgsmaprenderercache.h>
#include <qgsmaprendererparalleljob.h>

#include "mapengine.h"
#include "mapview.h"


MapImage::MapImage(QQuickItem* parent)
  : QQuickItem(parent)
  , mEngine(0)
  , mNewImg(false)
  , mRefreshRequested(false)
  , mJobCancelled(false)
  , mJob(0)
  , mCache(new QgsMapRendererCache())
{
  setFlag(ItemHasContents);
}

void MapImage::setMapEngine(MapEngine* e)
{
  mEngine = e;

  emit mapEngineChanged();
}

QSGNode* MapImage::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData*)
{
  if (!mEngine) { qDebug("no engine!"); return node; }

  QSGSimpleTextureNode *n = static_cast<QSGSimpleTextureNode *>(node);
  if (!n) {
      n = new QSGSimpleTextureNode();
      mNewImg = true;
  }

  if (mNewImg)
  {
    QSGTexture *texture = window()->createTextureFromImage(mMapImage);
    n->setTexture(texture);
    n->setOwnsTexture(true);
    mNewImg = false;
  }

  n->setRect(boundingRect());
  return n;
}



void MapImage::refreshMap(MapView* mv)
{
  if (!mEngine)
  {
    qDebug("no engine for refresh!");
    return;
  }

  mMapSettings = mEngine->mapSettings();
  mMapSettings.setOutputSize(mv->size());
  mMapSettings.setExtent(qrect2qgsrect(mv->toExtent()));

  if (!mMapSettings.hasValidSettings())
  {
    qDebug("invalid map settings - nothing to render");
    return;
  }

  if (!mRefreshRequested)
  {
    mRefreshRequested = true;
    QTimer::singleShot(0, this, SLOT(refreshMapDelayed()));
  }
}

void MapImage::refreshMapDelayed()
{
  mRefreshRequested = false;

  if (mJob)
  {
    qDebug("cancelling!");
    mJobCancelled = true;
    mJob->cancel();
  }

  mJobCancelled = false;

  qDebug("Starting job...");

  mJob = new QgsMapRendererParallelJob(mMapSettings);
  connect(mJob, SIGNAL(finished()), this, SLOT(jobFinished()));
  mJob->setCache(mCache);
  mJob->start();
}

void MapImage::jobFinished()
{
  Q_ASSERT(mJob);
  qDebug("job finished! %d", !mJobCancelled);

  if (!mJobCancelled)
  {
    mMapImage = mJob->renderedImage();
    mNewImg = true;
    update();
    emit mapImageChanged();
  }

  mJob->deleteLater();
  mJob = 0;
}
