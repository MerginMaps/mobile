#include "project.h"

#include <qgsproject.h>
#include <qgslayertree.h>

Project* Project::sInstance = 0;

Project::Project(QObject *parent)
  : QObject(parent)
{

}


Project* Project::instance()
{
  if (!sInstance)
    sInstance = new Project();
  return sInstance;
}

void Project::setProjectFile(const QString& filename)
{
  mFilename = filename;

  QgsProject::instance()->setFileName(filename);
  bool res = QgsProject::instance()->read();
  qDebug("load project: %d", res);

  emit projectFileChanged();
}

QList< QgsMapLayer* > Project::layers() const
{
  QgsLayerTreeGroup* root = QgsProject::instance()->layerTreeRoot();
  //qDebug("root: %s", root->dump().toLocal8Bit().data());
  QList< QgsMapLayer* > list;
  foreach (QgsLayerTreeLayer* nodeLayer, root->findLayers())
  {
    if (nodeLayer->isVisible())
      list << nodeLayer->layer();
  }

  return list;
}
