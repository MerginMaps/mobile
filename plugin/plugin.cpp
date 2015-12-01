#include "plugin.h"
#include "mapengine.h"
#include "mapimage.h"
#include "maplayer.h"
#include "mapview.h"
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
  if (!QgsApplication::createDB())
    qDebug("Can't create qgis user DB!!!");

  qDebug("qgis_init %f [s]", t.elapsed()/1000.0);
}



void Qgis_Mobile_ComponentsPlugin::registerTypes(const char *uri)
{
  qDebug("REGISTERING QGIS plugin");

  // @uri qgis
  qmlRegisterType<MapEngine>(uri, 1, 0, "MapEngine");
  qmlRegisterType<MapView>(uri, 1, 0, "MapView");
  qmlRegisterType<MapImage>(uri, 1, 0, "MapImage");
  qmlRegisterType<MapLayer>(uri, 1, 0, "MapLayer");
  qmlRegisterSingletonType<PositionEngine>(uri, 1, 0, "PositionEngine", _positionEngineProvider);
  qmlRegisterSingletonType<Project>(uri, 1, 0, "Project", _projectProvider);

  // init QGIS core library
  init_qgis();

  //qDebug("%s", QgsApplication::showSettings().toLocal8Bit().data());
  //qDebug("providers %s", QgsProviderRegistry::instance()->pluginList().toLocal8Bit().data());
}
