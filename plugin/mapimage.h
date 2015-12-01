#ifndef MAPIMAGE_H
#define MAPIMAGE_H

#include <QObject>
#include <QQuickItem>

#include "qgsmapsettings.h"

class QgsMapRendererCache;
class QgsMapRendererParallelJob;

class MapEngine;
class MapView;

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

#endif // MAPIMAGE_H
