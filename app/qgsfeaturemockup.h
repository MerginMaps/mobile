#ifndef QGSFEATUREMOCKUP_H
#define QGSFEATUREMOCKUP_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QList>

struct QgsFeatureMock {
  int id;
  QString name;
  double lon;
  double lat;
  QgsFeatureMock(int id, QString name, double lon, double lat) : id(id), name(name), lon(lon), lat(lat) {}
};

class QgsFeatureMockup : public QObject
{
  Q_OBJECT
public:
  QgsFeatureMockup();
  virtual ~QgsFeatureMockup() {}

  QList<QgsFeatureMock> getDataForLayer( const QString &layerName );

private:
  QHash<QString, QList<QgsFeatureMock>> _data;
};

#endif // QGSFEATUREMOCKUP_H
