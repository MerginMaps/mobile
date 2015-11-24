#include "plugin.h"
#include "mapengine.h"
#include "maplayer.h"
#include "positionengine.h"
#include "project.h"

#include <qgsapplication.h>
#include <qgsproviderregistry.h>

#include <qqml.h>

static QObject *_positionEngineProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
  Q_UNUSED(engine)
  Q_UNUSED(scriptEngine)
  return PositionEngine::instance();
}

static QObject *_projectProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
  Q_UNUSED(engine)
  Q_UNUSED(scriptEngine)
  return Project::instance();
}

void Qgis_Mobile_ComponentsPlugin::registerTypes(const char *uri)
{
  // @uri qgis
  qmlRegisterType<MapEngine>(uri, 1, 0, "MapEngine");
  qmlRegisterType<MapImage>(uri, 1, 0, "MapImage");
  qmlRegisterType<MapLayer>(uri, 1, 0, "MapLayer");
  qmlRegisterSingletonType<PositionEngine>(uri, 1, 0, "PositionEngine", _positionEngineProvider);
  qmlRegisterSingletonType<Project>(uri, 1, 0, "Project", _projectProvider);

  // init QGIS core library
  ::setenv("QGIS_PREFIX_PATH", QGIS_PREFIX_PATH, true);
  QgsApplication::init();
  QgsApplication::initQgis();
  //qDebug("%s", QgsApplication::showSettings().toLocal8Bit().data());
  //qDebug("providers %s", QgsProviderRegistry::instance()->pluginList().toLocal8Bit().data());
}


