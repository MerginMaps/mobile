#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "HotReload.h"
#include "Helper.h"
#include <QFont>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  app.setFont( QFont( Helper::installFonts() ) );

  QQmlApplicationEngine engine;
  HotReload hotReload(engine);
  engine.rootContext()->setContextProperty("_hotReload", &hotReload);
  // path to local wrapper pages
  engine.rootContext()->setContextProperty("_qmlWrapperPath", QGuiApplication::applicationDirPath() + "/HotReload/GalleryApp/qml/pages/");
  engine.rootContext()->setContextProperty( "__dp", Helper::calculateDpRatio() );

  QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
    &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.loadFromModule("GalleryApp", "Main");

  return app.exec();
}
