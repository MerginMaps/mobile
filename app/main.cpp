// Copyright 2017 Lutra Consulting Limited

#define STR1(x)  #x
#define STR(x)  STR1(x)

#include <QFontDatabase>
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
#include "qgsconfig.h"

#include "androidutils.h"
#include "inpututils.h"
#include "projectsmodel.h"
#include "layersmodel.h"
#include "mapthemesmodel.h"
#include "digitizingcontroller.h"
#include "merginapi.h"
#include "merginprojectmodel.h"

#include "qgsquickutils.h"
#include "qgsproject.h"

#ifndef NDEBUG
//#include <QQmlDebuggingEnabler>
#endif

#ifdef ANDROID
#include <QFile>
#include <QDir>
#include <QtAndroidExtras>
#endif

#ifndef ANDROID
#include <QCommandLineParser>
#include <qgis.h>
#endif

#include "qgsapplication.h"
#include "loader.h"
#include "appsettings.h"

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

  qDebug() << "QGIS_PREFIX_PATH: " << ::getenv("QGIS_PREFIX_PATH");
}

static bool cpDir(const QString &srcPath, const QString &dstPath)
{
    QDir parentDstDir(QFileInfo(dstPath).path());
    if (!parentDstDir.mkpath(dstPath))
        return false;

    QDir srcDir(srcPath);
    foreach(const QFileInfo &info, srcDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
        QString srcItemPath = srcPath + "/" + info.fileName();
        QString dstItemPath = dstPath + "/" + info.fileName();
        if (info.isDir()) {
            if (!cpDir(srcItemPath, dstItemPath)) {
                return false;
            }
        } else if (info.isFile()) {
            if (!QFile::copy(srcItemPath, dstItemPath)) {
                return false;
            }
            QFile::setPermissions(dstItemPath, QFile::ReadUser|QFile::WriteUser|QFile::ReadOwner|QFile::WriteOwner);
        } else {
            qDebug() << "Unhandled item" << info.filePath() << "in cpDir";
        }
    }
    return true;
}

// Copies resources folder to package folder
static void expand_pkg_data(const QString& pkgPath) {
#ifdef ANDROID
  QString assetsBasePath( "assets:" );
  cpDir(assetsBasePath + "/qgis-data", pkgPath);
#else
    Q_UNUSED(pkgPath);
#endif
}

static void copy_demo_projects(const QString& projectDir) {
#ifdef ANDROID
  QString assetsBasePath( "assets:" );
  qDebug("assets base path:  %s", assetsBasePath.toLatin1().data());
  cpDir(assetsBasePath + "/demo-projects", projectDir);
#else
    Q_UNUSED(projectDir);
#endif
}

static void init_qgis(const QString & pkgPath)
{
  QTime t;
  t.start();

  QgsApplication::init();
  QgsApplication::initQgis();

#ifdef ANDROID
  // QGIS plugins on Android are in the same path as other libraries
  QgsApplication::setPluginPath( QApplication::applicationDirPath() );
  QgsApplication::setPkgDataPath( pkgPath );
#else
  Q_UNUSED(pkgPath)
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
    qmlRegisterUncreatableType<AppSettings>("lc", 1, 0, "AppSettings", "");
    qmlRegisterType<DigitizingController>("lc", 1, 0, "DigitizingController");
}

int main(int argc, char *argv[])
{
  QgsApplication app(argc, argv, true);

  qDebug() << "Built with QGIS version " << VERSION_INT;
  // we ship our fonts because they do not need to be installed on the target platform
  QStringList fonts;
  fonts << ":/Lato-Regular.ttf"
        << ":/Lato-Bold.ttf";
  for (QString font : fonts)
  {
    if (QFontDatabase::addApplicationFont(font) == -1)
      qDebug() << "!! Failed to load font" << font;
    else
      qDebug() << "Loaded font" << font;
  }

  app.setFont(QFont("Lato"));

  // Require permissions before accessing data folder
#ifdef ANDROID
  AndroidUtils::requirePermissions();
#endif
  // Set/Get enviroment
  QString dataDir = getDataDir();
  QString projectDir = dataDir + "/projects";
  setEnvironmentQgisPrefixPath();

  init_qgis(dataDir + "/qgis-data");
  expand_pkg_data( QgsApplication::pkgDataPath() );
  copy_demo_projects( projectDir );
  QQmlEngine engine;
  engine.addImportPath( QgsApplication::qmlImportPath() );
  initDeclarative();

  // Set up the QSettings environment must be done after qapp is created
  QCoreApplication::setOrganizationName( "Lutra Consulting" );
  QCoreApplication::setOrganizationDomain( "lutraconsulting.co.uk" );
  QCoreApplication::setApplicationName( "Input" );
  QCoreApplication::setApplicationVersion("0.1");

  // QGIS environment variables to set
  // OGR_SQLITE_JOURNAL is set to DELETE to avoid working with WAL files
  // and properly close connection after writting changes to gpkg.
  ::setenv( "OGR_SQLITE_JOURNAL", "DELETE", 1 );

  // Create android utils
  AndroidUtils au;
  engine.rootContext()->setContextProperty( "__androidUtils", &au );

  // Create input utils
  InputUtils iu;
  engine.rootContext()->setContextProperty( "__inputUtils", &iu );

  // Create project model
  ProjectModel pm(projectDir);
  if (pm.rowCount() == 0) {
      qDebug() << "Unable to find any QGIS project in the folder " << projectDir;
  }
  engine.rootContext()->setContextProperty( "__projectsModel", &pm );

  // Create QGIS project
  Loader loader;
  engine.rootContext()->setContextProperty( "__loader", &loader );

  // Create layer model
  LayersModel lm(loader.project());
  engine.rootContext()->setContextProperty( "__layersModel", &lm );

  // Create map theme model
  MapThemesModel mtm(loader.project());
  engine.rootContext()->setContextProperty( "__mapThemesModel", &mtm );

  // Create app settings
  AppSettings as;
  engine.rootContext()->setContextProperty( "__appSettings", &as );

  // Create mergin api
  std::unique_ptr<MerginApi> ma =  std::unique_ptr<MerginApi>(new MerginApi( projectDir ));

  engine.rootContext()->setContextProperty( "__merginApi", ma.get() );

  // Create mergin projects model
  MerginProjectModel mpm;
  engine.rootContext()->setContextProperty( "__merginProjectsModel", &mpm );

  // Connections
  QObject::connect(&app, &QGuiApplication::applicationStateChanged, &loader, &Loader::appStateChanged);
  QObject::connect(&loader, &Loader::projectReloaded, &lm, &LayersModel::reloadLayers);
  QObject::connect(&loader, &Loader::projectReloaded, &mtm, &MapThemesModel::reloadMapThemes);
  QObject::connect(&mtm, &MapThemesModel::reloadLayers, &lm, &LayersModel::reloadLayers);
  QObject::connect(ma.get(), &MerginApi::syncProjectFinished, &mpm, &MerginProjectModel::syncProjectFinished);
  QObject::connect(ma.get(), &MerginApi::syncProjectFinished, &pm, &ProjectModel::addProject);
  QObject::connect(ma.get(), &MerginApi::listProjectsFinished, &mpm, &MerginProjectModel::resetProjects);
  QObject::connect(&pm, &ProjectModel::projectDeleted, ma.get(), &MerginApi::projectDeleted);

#ifdef ANDROID
  engine.rootContext()->setContextProperty( "__appwindowvisibility", "Maximized");
  engine.rootContext()->setContextProperty( "__appwindowwidth", 0);
  engine.rootContext()->setContextProperty( "__appwindowheight", 0);
#else
  engine.rootContext()->setContextProperty( "__appwindowvisibility", "windowed");
  engine.rootContext()->setContextProperty( "__appwindowwidth", 640 );
  engine.rootContext()->setContextProperty( "__appwindowheight", 1136 );
#endif

  // Set simulated position for desktop builds
#ifndef ANDROID
  bool use_simulated_position = true;
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

  #ifndef ANDROID
  QCommandLineParser parser;
  parser.addVersionOption();
  parser.process(app);
  #endif

  // Add some data for debugging if needed (visible in the final customer app)
  QgsApplication::messageLog()->logMessage(QgsQuickUtils().dumpScreenInfo());
  QgsApplication::messageLog()->logMessage("data directory: " + dataDir);
  QgsApplication::messageLog()->logMessage("All up and running");

#ifdef ANDROID
  QtAndroid::hideSplashScreen();
#endif

  return app.exec();
}


