#ifndef QGIS_MOBILE_COMPONENTS_PLUGIN_H
#define QGIS_MOBILE_COMPONENTS_PLUGIN_H

#include <QQmlExtensionPlugin>

class Qgis_Mobile_ComponentsPlugin : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
  void registerTypes(const char *uri);
};

#endif // QGIS_MOBILE_COMPONENTS_PLUGIN_H

