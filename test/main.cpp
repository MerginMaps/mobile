#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "qgsapplication.h"

static void init_qgis()
{
#ifndef ANDROID
  if (::getenv("QGIS_PREFIX_PATH") == 0)
  {
    qDebug("== Must set QGIS_PREFIX_PATH in order to get QGIS Quick module running! ==");
    return;
  }
#else
  if (::getenv("QGIS_ASSETS_URL") == 0)
  {
    qDebug("== Must set QGIS_ASSETS_URL in order to get QGIS Quick module running! ==");
    return;
  }
#endif

  QTime t;
  t.start();

#ifdef ANDROID
  QDir myDir( QDir::homePath() );
  myDir.cdUp();
  QString prefixPath = myDir.absolutePath();  // something like: /data/data/uk.co.lutraconsulting.manioc
  ::setenv("QGIS_PREFIX_PATH", prefixPath.toUtf8().constData(), true);
#endif

  // if not on Android, QGIS_PREFIX_PATH env variable should have been set already
  QgsApplication::init();
  QgsApplication::initQgis();

#ifdef ANDROID
  // QGIS plugins on Android are in the same path as other libraries
  QgsApplication::setPluginPath( QApplication::applicationDirPath() );

  QString qgisDataSubdir = "qgis-data";
  QDir homeDir = QDir::home();
  QString qgisDataPath = QDir::homePath() + "/" + qgisDataSubdir;
  QString assetsBasePath( ::getenv("QGIS_ASSETS_URL") );

  // make sure all necessary QGIS data are available - unpacked from assets
  //if (!homeDir.exists(qgisDataSubdir)) // do it always on startup - for now
  {
    bool res = homeDir.mkpath(qgisDataSubdir);
    qDebug("mkpath [%d] %s", res, qgisDataPath.toLatin1().data());
    QDir qgisDataDir(qgisDataPath);

    QStringList qgisDataFiles;
    qgisDataFiles << "resources/qgis.db" << "resources/srs.db";
    foreach (const QString& dataFile, qgisDataFiles)
    {
      QFile f(assetsBasePath + "/" + dataFile);
      QString destFilePath = qgisDataPath + "/" + dataFile;
      QDir destFileDir = QFileInfo(destFilePath).absoluteDir();
      if (!destFileDir.exists())
      {
        bool mkres = qgisDataDir.mkpath(QFileInfo(dataFile).dir().path());
        qDebug("mkpath [%d] %s", mkres, destFileDir.absolutePath().toLatin1().data());
      }
      if (!QFile(destFilePath).exists())
      {
        bool res = f.copy(destFilePath);
        qDebug("copying [%d] %s", res, destFilePath.toLatin1().data());
        // by default the permissions are set as readable for everyone - and not writable!
        res = QFile::setPermissions(destFilePath, QFile::ReadUser|QFile::WriteUser|QFile::ReadOwner|QFile::WriteOwner);
        qDebug("chmod [%d]", res);
      }
    }
  }

  QgsApplication::setPkgDataPath(qgisDataPath);
#endif

  // make sure the DB exists - otherwise custom projections will be failing
  if (!QgsApplication::createDatabase())
    qDebug("Can't create qgis user DB!!!");

  qDebug("qgis_init %f [s]", t.elapsed()/1000.0);
}

int main(int argc, char *argv[])
{
  QgsApplication app(argc, argv, true);

#ifdef QGIS_PREFIX_PATH
  ::setenv("QGIS_PREFIX_PATH", QGIS_PREFIX_PATH, true);
#endif
  init_qgis();
  //qDebug("%s", QgsApplication::showSettings().toLocal8Bit().data());
  //qDebug("providers %s", QgsProviderRegistry::instance()->pluginList().toLocal8Bit().data());

  QQmlApplicationEngine engine;

  // if the QML module is in different path
  //engine.addImportPath( PATH );

  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

  return app.exec();
}

