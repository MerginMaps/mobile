#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "HotReload.h"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  HotReload hotReload(engine);
  engine.rootContext()->setContextProperty("_hotReload", &hotReload);
  // path to local wrapper pages
  engine.rootContext()->setContextProperty("_qmlWrapperPath", QGuiApplication::applicationDirPath() + "/HotReload/GalleryApp/qml/pages/");
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
    &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.loadFromModule("GalleryApp", "Main");
  return app.exec();
}
