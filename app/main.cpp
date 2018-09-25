// Copyright 2017 Lutra Consulting Limited

#define STR1(x)  #x
#define STR(x)  STR1(x)

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QtDebug>
#include <QQmlError>
#include <QDesktopWidget>
#include <QQmlContext>
#include <QQuickWindow>
#include <qqml.h>
#include <qgsmessagelog.h>

#include "projectsmodel.h"
#include "layersmodel.h"
#include "digitizingcontroller.h"

#include "qgsquickutils.h"
#include "qgsproject.h"

#ifndef NDEBUG
//#include <QQmlDebuggingEnabler>
#endif

#ifdef ANDROID
#include <QFile>
#include <QDir>
#endif

#ifndef ANDROID
#include <QCommandLineParser>
#include <qgis.h>
#endif

#include "qgsapplication.h"
#include "loader.h"

static QString getDataDir() {
#ifdef QGIS_QUICK_DATA_PATH
  QString dataPathRaw(STR(QGIS_QUICK_DATA_PATH));

#ifdef ANDROID
  QFileInfo extDir("/sdcard/");
  if(extDir.isDir() && extDir.isWritable()){
      // seems that this directory transposes to the latter one in case there is no sdcard attached
      dataPathRaw = extDir.path() + "/" + dataPathRaw;
  } else {
      qDebug() << "extDir: " << extDir.path() << " not writable";

      QStringList split = QDir::homePath().split("/"); // something like /data/user/0/uk.co.lutraconsulting/files
      // TODO support active user from QDir::homePath()
      QFileInfo usrDir("/storage/emulated/" + split[2] + "/");
      dataPathRaw = usrDir.path() + "/" + dataPathRaw;
      if(!(usrDir.isDir() && usrDir.isWritable())){
          qDebug() << "usrDir: " << usrDir.path() << " not writable";
      }
  }
#endif

  ::setenv("QGIS_QUICK_DATA_PATH", dataPathRaw.toUtf8().constData(), true);
#else
  qDebug("== Must set QGIS_QUICK_DATA_PATH in order to get QGIS Quick running! ==");
#endif
  QString dataDir(::getenv("QGIS_QUICK_DATA_PATH"));
  qDebug() << "QGIS_QUICK_DATA_PATH: " << dataDir;
  return dataDir;
}

static void setEnvironmentQgisPrefixPath() {
#ifndef ANDROID
#ifdef QGIS_PREFIX_PATH
  ::setenv("QGIS_PREFIX_PATH", STR(QGIS_PREFIX_PATH), true);
#endif
  if (::getenv("QGIS_PREFIX_PATH") == 0)
  {
    // if not on Android, QGIS_PREFIX_PATH env variable should have been set already or defined as C++ define
    qDebug("== Must set QGIS_PREFIX_PATH in order to get QGIS Quick module running! ==");
  }
#endif

#ifdef ANDROID
  QDir myDir( QDir::homePath() );
  myDir.cdUp();
  QString prefixPath = myDir.absolutePath();  // something like: /data/data/org.qgis.quick
  ::setenv("QGIS_PREFIX_PATH", prefixPath.toUtf8().constData(), true);
#endif
}


static void expand_assets_data(const QString& qgisDataPath) {
#ifdef ANDROID
  QString assetsBasePath( "assets:" );
  qDebug("assets base path:  %s", assetsBasePath.toLatin1().data());

  QDir qgisDataDir(qgisDataPath);

  // make sure all necessary QGIS data are available - unpacked from assets
  //if (!homeDir.exists(qgisDataSubdir)) // do it always on startup - for now
  {
    QStringList qgisDataFiles;
    qgisDataFiles << "resources/qgis.db" << "resources/srs.db";

#ifdef QGIS_QUICK_EXPAND_TEST_DATA
    qgisDataFiles << "flood damage/point damage.prj";
    qgisDataFiles << "flood damage/tree damage.shx";
    qgisDataFiles << "flood damage/tree damage.prj";
    qgisDataFiles << "flood damage/fence damage.shx";
    qgisDataFiles << "flood damage/point damage.qpj";
    qgisDataFiles << "flood damage/tree damage.shp";
    qgisDataFiles << "flood damage/tree damage.qpj";
    qgisDataFiles << "flood damage/flood damage.qgs";
    qgisDataFiles << "flood damage/fence damage.qpj";
    qgisDataFiles << "flood damage/point damage.shx";
    qgisDataFiles << "flood damage/fence damage.shp";
    qgisDataFiles << "flood damage/fence damage.dbf";
    qgisDataFiles << "flood damage/tree damage.dbf";
    qgisDataFiles << "flood damage/fence damage.prj";
    qgisDataFiles << "flood damage/point damage.shp";
    qgisDataFiles << "flood damage/point damage.dbf";
    qgisDataFiles << "wildlife/wildlife.qgs";
    qgisDataFiles << "wildlife/pastures.dbf";
    qgisDataFiles << "wildlife/pastures.qpj";
    qgisDataFiles << "wildlife/pastures.shp";
    qgisDataFiles << "wildlife/animal count.prj";
    qgisDataFiles << "wildlife/fences.prj";
    qgisDataFiles << "wildlife/pastures.shx";
    qgisDataFiles << "wildlife/fences.shx";
    qgisDataFiles << "wildlife/animal count.shp";
    qgisDataFiles << "wildlife/fences.shp";
    qgisDataFiles << "wildlife/pastures.prj";
    qgisDataFiles << "wildlife/animal count.shx";
    qgisDataFiles << "wildlife/animal count.qpj";
    qgisDataFiles << "wildlife/animal count.dbf";
    qgisDataFiles << "wildlife/fences.qpj";
    qgisDataFiles << "wildlife/fences.dbf";
    qgisDataFiles << "background.gpkg";
    qgisDataFiles << "tracks conditions/RoW_infra.dbf";
    qgisDataFiles << "tracks conditions/RoW_infra.prj";
    qgisDataFiles << "tracks conditions/track conditions.qgs";
    qgisDataFiles << "tracks conditions/Green_lane_condition_surveys.prj";
    qgisDataFiles << "tracks conditions/Green_lane_condition_surveys.qpj";
    qgisDataFiles << "tracks conditions/Green_lane_condition_surveys.shp";
    qgisDataFiles << "tracks conditions/RoW_infra.qpj";
    qgisDataFiles << "tracks conditions/RoW_infra.shx";
    qgisDataFiles << "tracks conditions/RoW_infra.cpg";
    qgisDataFiles << "tracks conditions/Route_accessibility.dbf";
    qgisDataFiles << "tracks conditions/Green_lane_condition_surveys.dbf";
    qgisDataFiles << "tracks conditions/Green_lane_condition_surveys.shx";
    qgisDataFiles << "tracks conditions/Route_accessibility.shp";
    qgisDataFiles << "tracks conditions/Route_accessibility.prj";
    qgisDataFiles << "tracks conditions/RoW_infra.shp";
    qgisDataFiles << "tracks conditions/Green_lane_condition_surveys.cpg";
    qgisDataFiles << "tracks conditions/Route_accessibility.shx";
    qgisDataFiles << "tracks conditions/Route_accessibility.qpj";
#endif

    foreach (const QString& dataFile, qgisDataFiles)
    {
      QFile f(assetsBasePath + "/qgis-data/" + dataFile);
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
#else
    Q_UNUSED(qgisDataPath);
#endif
}

static void init_qgis(const QString& qgisDataPath)
{
  QTime t;
  t.start();

  QgsApplication::init();
  QgsApplication::initQgis();

#ifdef ANDROID
  // QGIS plugins on Android are in the same path as other libraries
  QgsApplication::setPluginPath( QApplication::applicationDirPath() );
  QgsApplication::setPkgDataPath(qgisDataPath);
#else
  Q_UNUSED(qgisDataPath);
#endif

  // make sure the DB exists - otherwise custom projections will be failing
  if (!QgsApplication::createDatabase())
    qDebug("Can't create qgis user DB!!!");

  qDebug("qgis_init %f [s]", t.elapsed()/1000.0);
}

void initDeclarative() {
    qmlRegisterUncreatableType<ProjectModel>( "lc", 1, 0, "ProjectModel", "" );
    qmlRegisterUncreatableType<LayersModel>( "lc", 1, 0, "LayersModel", "" );
    qmlRegisterUncreatableType<Loader>("lc", 1, 0, "Loader", "");
    qmlRegisterType<DigitizingController>("lc", 1, 0, "DigitizingController");
}

int show_error(const QgsApplication& app,
                QQmlEngine& engine,
                const QString& msg)
{
    engine.rootContext()->setContextProperty( "__msg", msg );
    QQmlComponent component(&engine, QUrl("qrc:/error.qml"));
    QObject *object = component.create();
    Q_ASSERT(object);
    if ( QQuickWindow* quickWindow = qobject_cast<QQuickWindow*>( object ) )
    {
      quickWindow->setIcon(QIcon(":/logo.png"));
    }
    return app.exec();
}

int main(int argc, char *argv[])
{
  QgsApplication app(argc, argv, true);

  // Set/Get enviroment
  QString dataDir = getDataDir();
  setEnvironmentQgisPrefixPath();
  expand_assets_data(dataDir);

  init_qgis(dataDir);
  QQmlEngine engine;
  initDeclarative();

  // Create project model
  ProjectModel pm(dataDir);
  if (pm.rowCount() == 0) {
#ifdef ANDROID
      QString msg;
      msg += "Unable to find any QGIS project. \n\n";
      msg += "Please copy QGIS project (.qgs) with all layers to the mobile device and restart the application:\n";
      msg += "  if you have SD card, location is INPUT under SD card base folder\n";
      msg += "  if you do NOT have SD card, location is INPUT under your user home folder\n\n";
      msg += "Folder INPUT should already be created and should contain QGIS resources directory.";
      show_error(app, engine, msg);
#else
  #ifndef NDEBUG
      QString msg("Unable to find any QGIS project \n in folder " + dataDir);
      show_error(app, engine, msg);
  #else
      qDebug() << "Unable to find any QGIS project in the folder " << dataDir
  #endif
#endif
      return EXIT_FAILURE;
  }
  engine.rootContext()->setContextProperty( "__projectsModel", &pm );

  // Create QGIS project
  Loader loader;
  engine.rootContext()->setContextProperty( "__loader", &loader );

  // Create layer model 
  LayersModel lm(loader.project());
  engine.rootContext()->setContextProperty( "__layersModel", &lm );

  // Connections
  QObject::connect(&loader, &Loader::projectReloaded, &lm, &LayersModel::reloadLayers);

  // Set Device Pixels
#ifdef ANDROID
  //float dp = calculateDevicePixels();
  engine.rootContext()->setContextProperty( "__appwindowvisibility", "Maximized");
  engine.rootContext()->setContextProperty( "__appwindowwidth", 0);
  engine.rootContext()->setContextProperty( "__appwindowheight", 0);
#else


  engine.rootContext()->setContextProperty( "__appwindowvisibility", "windowed");
  engine.rootContext()->setContextProperty( "__appwindowwidth", 1217);
  engine.rootContext()->setContextProperty( "__appwindowheight", 800);
#endif

  // Set simulated position for desktop builds
#ifndef ANDROID
  bool use_simulated_position = false;
#else
  bool use_simulated_position = false;
#endif
  engine.rootContext()->setContextProperty( "__use_simulated_position", use_simulated_position );

  QQmlComponent component(&engine, QUrl("qrc:/main.qml"));
  QObject *object = component.create();

  if (!component.errors().isEmpty()) {
      qDebug("%s", QgsApplication::showSettings().toLocal8Bit().data());

      qDebug() << "****************************************";
      qDebug() << "*****        QML errors:           *****";
      qDebug() << "****************************************";
      for(const QQmlError& error: component.errors()) {
        qDebug() << "  " << error;
      }
      qDebug() << "****************************************";
      qDebug() << "****************************************";
  }

  if( object == 0 )
  {
      qDebug() << "FATAL ERROR: unable to create main.qml";
      return EXIT_FAILURE;
  }

  if ( QQuickWindow* quickWindow = qobject_cast<QQuickWindow*>( object ) )
  {
    quickWindow->setIcon(QIcon(":/logo.png"));
  }

  // Set up the QSettings environment must be done after qapp is created
  QCoreApplication::setOrganizationName( "Lutra Consulting" );
  QCoreApplication::setOrganizationDomain( "lutraconsulting.co.uk" );
  QCoreApplication::setApplicationName( "Input" );
  QCoreApplication::setApplicationVersion("0.1");

  #ifndef ANDROID
  QCommandLineParser parser;
  parser.addVersionOption();
  parser.process(app);
  #endif

  // Add some data for debugging if needed (visible in the final customer app)
  QRect rec = QApplication::desktop()->screenGeometry();
  int height = rec.height();
  int width = rec.width();
  QgsApplication::messageLog()->logMessage(QgsQuickUtils().dumpScreenInfo());
  QgsApplication::messageLog()->logMessage("data directory: " + dataDir);
  QgsApplication::messageLog()->logMessage("All up and running");

  return app.exec();
}


