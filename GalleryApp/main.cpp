#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "HotReload.h"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  HotReload hotReload(engine, "HotReload/");
  engine.rootContext()->setContextProperty("_hotReload", &hotReload);
  engine.rootContext()->setContextProperty("_qmlPath", QGuiApplication::applicationDirPath() + "/HotReload/GalleryApp/qml/pages/");
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
    &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.loadFromModule("GalleryApp", "Main");
  return app.exec();
}
