#ifndef MAPLAYER_H
#define MAPLAYER_H

#include <QObject>

class QgsRasterLayer;

class MapLayer : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
  Q_PROPERTY(QString provider READ provider WRITE setProvider NOTIFY providerChanged)
  Q_PROPERTY(QString layerId READ layerId NOTIFY layerLoaded)
public:
  MapLayer();

  QString source() const { return mSource; }
  QString provider() const { return mProvider; }
  QString layerId() const;

  void setSource(const QString& src);
  void setProvider(const QString& prv);

signals:
  void sourceChanged();
  void providerChanged();
  void layerLoaded();

protected:
  void load();

private:
  QString mSource;
  QString mProvider;

  QgsRasterLayer* mRL;
};

#endif // MAPLAYER_H
