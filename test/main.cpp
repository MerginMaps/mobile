#include <QGuiApplication>
#include <QQmlApplicationEngine>


int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

#ifdef QGIS_PREFIX_PATH
  ::setenv("QGIS_PREFIX_PATH", QGIS_PREFIX_PATH, true);
#endif

  QQmlApplicationEngine engine;

  // if the QML module is in different path
  //engine.addImportPath( PATH );

  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

  return app.exec();
}

