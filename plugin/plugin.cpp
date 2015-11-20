#include "plugin.h"
#include "mapengine.h"
#include "maplayer.h"
#include "positionengine.h"

#include <qgsapplication.h>

#include <qqml.h>

static QObject *_positionEngineProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
  Q_UNUSED(engine)
  Q_UNUSED(scriptEngine)
  return PositionEngine::instance();
}

void Qgis_Mobile_ComponentsPlugin::registerTypes(const char *uri)
{
  // @uri qgis
  qmlRegisterType<MapEngine>(uri, 1, 0, "MapEngine");
  qmlRegisterType<MapImage>(uri, 1, 0, "MapImage");
  qmlRegisterType<MapLayer>(uri, 1, 0, "MapLayer");
  qmlRegisterSingletonType<PositionEngine>(uri, 1, 0, "PositionEngine", _positionEngineProvider);

  // init QGIS core library
  ::setenv("QGIS_PREFIX_PATH", QGIS_PREFIX_PATH, true);
  QgsApplication::init();
  QgsApplication::initQgis();

}


