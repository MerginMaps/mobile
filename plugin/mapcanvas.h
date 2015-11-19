#ifndef MAPCANVAS_H
#define MAPCANVAS_H

#include <QQuickItem>

class MapCanvas : public QQuickItem
{
  Q_OBJECT
  Q_DISABLE_COPY(MapCanvas)

public:
  MapCanvas(QQuickItem *parent = 0);
  ~MapCanvas();
};

#endif // MAPCANVAS_H

