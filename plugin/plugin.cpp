#include "plugin.h"
#include "mapcanvas.h"

#include <qqml.h>

void Qgis_Mobile_ComponentsPlugin::registerTypes(const char *uri)
{
  // @uri qgis
  qmlRegisterType<MapCanvas>(uri, 1, 0, "MapCanvas");
}


