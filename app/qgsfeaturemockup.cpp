#include "qgsfeaturemockup.h"
#include <QDebug>

QgsFeatureMockup::QgsFeatureMockup()
{
  _data.insert("A", {
    QgsFeatureMock(0, "In a line", .3, .4),
    QgsFeatureMock(1, "In a point", .32, .41)
  });

  _data.insert("B", {
    QgsFeatureMock(0, "In b line", .19, .403231),
    QgsFeatureMock(1, "In b point", .7752, .214112)
  });
}

QList<QgsFeatureMock> QgsFeatureMockup::getDataForLayer( const QString &layerName )
{
  auto it = _data.find( layerName );

  if ( it.key() == layerName )
    return it.value();

  qDebug() << "Could not find data for layer " << layerName;
  return QList<QgsFeatureMock>();
}

