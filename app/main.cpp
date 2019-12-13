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
#include <QtGlobal>
#include <QQmlContext>
#include <QQuickWindow>
#ifdef INPUT_TEST
#include <QTest>
#endif
#include <qqml.h>
#include <qgsmessagelog.h>
#include "qgsconfig.h"
#include "qgsproviderregistry.h"

#include "androidutils.h"
#include "ios/iosutils.h"
#include "inpututils.h"
#include "projectsmodel.h"
#include "layersmodel.h"
#include "mapthemesmodel.h"
#include "digitizingcontroller.h"
#include "merginapi.h"
#include "merginapistatus.h"
#include "merginprojectmodel.h"
#include "merginprojectstatusmodel.h"
#include "ios/iosimagepicker.h"

#ifdef INPUT_TEST
#include "test/testmerginapi.h"
#endif

#include "qgsquickutils.h"
#include "qgsproject.h"

#ifndef NDEBUG
// #include <QQmlDebuggingEnabler>
#endif

#ifdef MOBILE_OS
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#endif

#ifdef ANDROID
#include <QtAndroidExtras>
#endif

#ifdef DESKTOP_OS
#include <QCommandLineParser>
#include <qgis.h>
#endif

#include "qgsapplication.h"
#include "loader.h"
#include "appsettings.h"

#ifdef Q_OS_IOS
#include "qgsquickplugin.h"
#endif

static QString getDataDir()
{
#ifdef QGIS_QUICK_DATA_PATH
  QString dataPathRaw( STR( QGIS_QUICK_DATA_PATH ) );

#ifdef ANDROID
  QFileInfo extDir( "/sdcard/" );
  if ( extDir.isDir() && extDir.isWritable() )
  {
    // seems that this directory transposes to the latter one in case there is no sdcard attached
    dataPathRaw = extDir.path() + "/" + dataPathRaw;
  }
  else
  {
    qDebug() << "extDir: " << extDir.path() << " not writable";

    QStringList split = QDir::homePath().split( "/" ); // something like /data/user/0/uk.co.lutraconsulting/files
    // TODO support active user from QDir::homePath()
    QFileInfo usrDir( "/storage/emulated/" + split[2] + "/" );
    dataPathRaw = usrDir.path() + "/" + dataPathRaw;
    if ( !( usrDir.isDir() && usrDir.isWritable() ) )
    {
      qDebug() << "usrDir: " << usrDir.path() << " not writable";
    }
  }
#endif

#ifdef Q_OS_IOS
  QString docsLocation = QStandardPaths::standardLocations( QStandardPaths::DocumentsLocation ).value( 0 );

  QDir myDir( docsLocation );
  if ( !myDir.exists() )
  {
    myDir.mkpath( docsLocation );
  }
  dataPathRaw = docsLocation + "/" + dataPathRaw;
#endif

#ifdef Q_OS_WIN32
  QString appLocation = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );

  QDir myDir( appLocation );
  if ( !myDir.exists() )
  {
    myDir.mkpath( appLocation );
  }
  dataPathRaw = appLocation + "/" + dataPathRaw;
#endif

  qputenv( "QGIS_QUICK_DATA_PATH", dataPathRaw.toUtf8().constData() );
#else
  qDebug( "== Must set QGIS_QUICK_DATA_PATH in order to get QGIS Quick running! ==" );
#endif
  QString dataDir = QString::fromLocal8Bit( qgetenv( "QGIS_QUICK_DATA_PATH" ) ) ;
  qDebug() << "QGIS_QUICK_DATA_PATH: " << dataDir;
  return dataDir;
}

static void setEnvironmentQgisPrefixPath()
{
#ifdef DESKTOP_OS
#ifdef QGIS_PREFIX_PATH
  qputenv( "QGIS_PREFIX_PATH", STR( QGIS_PREFIX_PATH ) );
#endif
  if ( QString::fromLocal8Bit( qgetenv( "QGIS_PREFIX_PATH" ) ).isEmpty() )
  {
    // if not on Android, QGIS_PREFIX_PATH env variable should have been set already or defined as C++ define
    qDebug( "== Must set QGIS_PREFIX_PATH in order to get QGIS Quick module running! ==" );
  }
#endif

#if defined (ANDROID) || defined (Q_OS_IOS)
  QDir myDir( QDir::homePath() );
  myDir.cdUp();
  QString prefixPath = myDir.absolutePath();  // something like: /data/data/org.qgis.quick
  qputenv( "QGIS_PREFIX_PATH", prefixPath.toUtf8().constData() );
#elif defined (Q_OS_WIN32)
  QString prefixPath = QCoreApplication::applicationDirPath();
  qputenv( "QGIS_PREFIX_PATH", prefixPath.toUtf8().constData() );
#endif

  qDebug() << "QGIS_PREFIX_PATH: " << QString::fromLocal8Bit( qgetenv( "QGIS_PREFIX_PATH" ) );
}

// Copies resources folder to package folder
static void expand_pkg_data( const QString &pkgPath )
{

#if defined (ANDROID)
  QString assetsBasePath( "assets:" );
  InputUtils::cpDir( assetsBasePath + "/qgis-data", pkgPath );
#else
  Q_UNUSED( pkgPath );
#endif
// on IOS and WIN32 the files are already in the bundle
}

static void copy_demo_projects( const QString &projectDir )
{
#if defined (ANDROID) || defined (Q_OS_IOS)
  QString assetsBasePath( "assets:" );
  qDebug( "assets base path:  %s", assetsBasePath.toLatin1().data() );
  InputUtils::cpDir( assetsBasePath + "/demo-projects", projectDir );
#elif defined (Q_OS_WIN32)
  InputUtils::cpDir( QCoreApplication::applicationDirPath() + "/demo-projects", projectDir );
#else
  Q_UNUSED( projectDir );
#endif
}

static void init_qgis( const QString &pkgPath )
{
  QTime t;
  t.start();

#ifdef Q_OS_WIN32
// PROJ3
  QString prefixPath = QCoreApplication::applicationDirPath() + "\\share\\proj";
  qputenv( "PROJ_LIB", prefixPath.toUtf8().constData() );
#endif

  QgsApplication::init();

#ifdef MOBILE_OS
  // QGIS plugins on Android are in the same path as other libraries
  QgsApplication::setPluginPath( QApplication::applicationDirPath() );
  QgsApplication::setPkgDataPath( pkgPath );
#else
  Q_UNUSED( pkgPath )
#endif

  QgsApplication::initQgis();

  // make sure the DB exists - otherwise custom projections will be failing
  if ( !QgsApplication::createDatabase() )
    qDebug( "Can't create qgis user DB!!!" );

  qDebug( "qgis_init %f [s]", t.elapsed() / 1000.0 );
  qDebug( "qgis providers:\n%s", QgsProviderRegistry::instance()->pluginList().toLatin1().data() );
}

void initDeclarative()
{
  qmlRegisterUncreatableType<ProjectModel>( "lc", 1, 0, "ProjectModel", "" );
  qmlRegisterUncreatableType<LayersModel>( "lc", 1, 0, "LayersModel", "" );
  qmlRegisterUncreatableType<MapThemesModel>( "lc", 1, 0, "MapThemesModel", "" );
  qmlRegisterUncreatableType<Loader>( "lc", 1, 0, "Loader", "" );
  qmlRegisterUncreatableType<AppSettings>( "lc", 1, 0, "AppSettings", "" );
  qmlRegisterUncreatableType<MerginApiStatus>( "lc", 1, 0, "MerginApiStatus", "MerginApiStatus Enum" );
  qmlRegisterUncreatableType<MerginProjectStatusModel>( "lc", 1, 0, "MerginProjectStatusModel", "Enum" );
  qmlRegisterType<DigitizingController>( "lc", 1, 0, "DigitizingController" );
  qmlRegisterType<IOSImagePicker>( "lc", 1, 0, "IOSImagePicker" );
}

#ifdef INPUT_TEST
void initTestDeclarative()
{
  qRegisterMetaType<MerginProjectList>( "MerginProjectList" );
}
#endif

int main( int argc, char *argv[] )
{
  QgsApplication app( argc, argv, true );

  QString version;
#ifdef INPUT_VERSION
  version = STR( INPUT_VERSION );
#endif

  // Set up the QSettings environment must be done after qapp is created
  QCoreApplication::setOrganizationName( "Lutra Consulting" );
  QCoreApplication::setOrganizationDomain( "lutraconsulting.co.uk" );
  QCoreApplication::setApplicationName( "Input" );
  QCoreApplication::setApplicationVersion( version );

#ifdef INPUT_TEST
  bool IS_TEST = false;
  for ( int i = 0; i < argc; ++i )
  {
    if ( std::string( argv[i] ) == "--test" ) IS_TEST = true;
  }
#endif
  qDebug() << "Built with QGIS version " << VERSION_INT;

  // Require permissions before accessing data folder
#ifdef ANDROID
  AndroidUtils::requirePermissions();
#endif
  // Set/Get enviroment
  QString dataDir = getDataDir();
  QString projectDir = dataDir + "/projects";

#ifdef INPUT_TEST
  if ( IS_TEST )
  {
    // override the path where local projects are stored
    // and wipe the temporary projects dir if it already exists
    QDir testDataDir( STR( INPUT_TEST_DATA_DIR ) );  // #defined in input.pro
    QDir testProjectsDir( testDataDir.path() + "/../temp_projects" );
    if ( testProjectsDir.exists() )
      testProjectsDir.removeRecursively();
    QDir( testDataDir.path() + "/.." ).mkpath( "temp_projects" );
    projectDir = testProjectsDir.canonicalPath();
  }
#endif

  InputUtils::setLogFilename( projectDir + "/.logs" );
  setEnvironmentQgisPrefixPath();

  QString appBundleDir;
#ifdef ANDROID
  appBundleDir = dataDir;
#endif
#ifdef Q_OS_IOS
  appBundleDir = QCoreApplication::applicationDirPath();
#endif
  init_qgis( QString( "%1/qgis-data" ).arg( appBundleDir ) );
  expand_pkg_data( QgsApplication::pkgDataPath() );

  // Create Input classes
  AndroidUtils au;
  IosUtils iosUtils;
  InputUtils iu;
  LocalProjectsManager localProjects( projectDir );
  ProjectModel pm( localProjects );
  Loader loader;
  AppSettings as;
  LayersModel lm( loader.project() );
  MapThemesModel mtm( loader.project() );
  std::unique_ptr<MerginApi> ma =  std::unique_ptr<MerginApi>( new MerginApi( localProjects ) );
  MerginProjectModel mpm( localProjects );
  MerginProjectStatusModel mpsm( localProjects );

  // Connections
  QObject::connect( &app, &QGuiApplication::applicationStateChanged, &loader, &Loader::appStateChanged );
  QObject::connect( &mtm, &MapThemesModel::reloadLayers, &lm, &LayersModel::reloadLayers );
  QObject::connect( ma.get(), &MerginApi::syncProjectFinished, &pm, &ProjectModel::addProject );
  QObject::connect( ma.get(), &MerginApi::listProjectsFinished, &mpm, &MerginProjectModel::resetProjects );
  QObject::connect( ma.get(), &MerginApi::syncProjectStatusChanged, &mpm, &MerginProjectModel::syncProjectStatusChanged );
  QObject::connect( ma.get(), &MerginApi::reloadProject, &loader, &Loader::reloadProject );

  QObject::connect( &loader, &Loader::projectReloaded, &mtm, &MapThemesModel::reloadMapThemes );
  QObject::connect( &mtm, &MapThemesModel::mapThemesChanged, &lm, &LayersModel::reloadLayers );
  QObject::connect( &mtm, &MapThemesModel::mapThemeChanged, &as, &AppSettings::setDefaultMapTheme );
  QObject::connect( &lm, &LayersModel::layersChanged, &as, &AppSettings::reloadDefaultLayer );
  QObject::connect( &as, &AppSettings::reloadDefaultLayerSignal, &lm, &LayersModel::updateActiveLayer );
  QObject::connect( &lm, &LayersModel::activeLayerNameChanged, &as, &AppSettings::setDefaultLayer );

  QFile projectLoadingFile( Loader::LOADING_FLAG_FILE_PATH );
  if ( projectLoadingFile.exists() )
  {
    // Cleaning default project due to a project loading has crashed during the last run.
    as.setDefaultProject( QString() );
    projectLoadingFile.remove();
    InputUtils::log( QStringLiteral( "Loading project error" ), QStringLiteral( "The Input has been unexpectedly finished during the last run." ) );
  }

#ifdef INPUT_TEST
  if ( IS_TEST )
  {
    initTestDeclarative();
    TestMerginApi test( ma.get(), &mpm, &pm );
    // use command line args we got, but filter out "--test" that's recognized by us but not by QTest framework
    // (command line args may be used to filter function names that should be executed)
    QVector<char *> args;
    for ( int i = 0; i < argc; ++i )
    {
      if ( QString( argv[i] ) != "--test" )
        args << argv[i];
    }
    QTest::qExec( &test, args.count(), args.data() );
    return 0;
  }
#endif

  // we ship our fonts because they do not need to be installed on the target platform
  QStringList fonts;
  fonts << ":/Lato-Regular.ttf"
        << ":/Lato-Bold.ttf";
  for ( QString font : fonts )
  {
    if ( QFontDatabase::addApplicationFont( font ) == -1 )
      qDebug() << "!! Failed to load font" << font;
    else
      qDebug() << "Loaded font" << font;
  }
  app.setFont( QFont( "Lato" ) );

  copy_demo_projects( projectDir );

#ifdef Q_OS_IOS
  // REQUIRED FOR IOS  - to load QgsQuick C++ classes
  QgsQuickPlugin plugin;
  plugin.registerTypes( "QgsQuick" );
#endif

  QQmlEngine engine;
  engine.addImportPath( QgsApplication::qmlImportPath() );
#ifdef Q_OS_IOS
  // REQUIRED FOR IOS - to load QgsQuick/*.qml files defined in qmldir
  engine.addImportPath( "qrc:///" );
#endif
  initDeclarative();

  // QGIS environment variables to set
  // OGR_SQLITE_JOURNAL is set to DELETE to avoid working with WAL files
  // and properly close connection after writting changes to gpkg.
  qputenv( "OGR_SQLITE_JOURNAL", "DELETE" );

  // Register to QQmlEngine
  engine.rootContext()->setContextProperty( "__androidUtils", &au );
  engine.rootContext()->setContextProperty( "__iosUtils", &iosUtils );
  engine.rootContext()->setContextProperty( "__inputUtils", &iu );
  engine.rootContext()->setContextProperty( "__projectsModel", &pm );
  engine.rootContext()->setContextProperty( "__loader", &loader );
  engine.rootContext()->setContextProperty( "__layersModel", &lm );
  engine.rootContext()->setContextProperty( "__mapThemesModel", &mtm );
  engine.rootContext()->setContextProperty( "__appSettings", &as );
  engine.rootContext()->setContextProperty( "__merginApi", ma.get() );
  engine.rootContext()->setContextProperty( "__merginProjectsModel", &mpm );
  engine.rootContext()->setContextProperty( "__merginProjectStatusModel", &mpsm );

#ifdef MOBILE_OS
  engine.rootContext()->setContextProperty( "__appwindowvisibility", "Maximized" );
  engine.rootContext()->setContextProperty( "__appwindowwidth", QVariant( 0 ) );
  engine.rootContext()->setContextProperty( "__appwindowheight", QVariant( 0 ) );
#else
  engine.rootContext()->setContextProperty( "__appwindowvisibility", "windowed" );
  engine.rootContext()->setContextProperty( "__appwindowwidth", 640 );
  engine.rootContext()->setContextProperty( "__appwindowheight", 1136 );
#endif
  engine.rootContext()->setContextProperty( "__version", version );

  // Set simulated position for desktop builds
#ifdef DESKTOP_OS
  bool use_simulated_position = true;
#else
  bool use_simulated_position = false;
#endif
  engine.rootContext()->setContextProperty( "__use_simulated_position", use_simulated_position );

  QQmlComponent component( &engine, QUrl( "qrc:/main.qml" ) );
  QObject *object = component.create();

  if ( !component.errors().isEmpty() )
  {
    qDebug( "%s", QgsApplication::showSettings().toLocal8Bit().data() );

    qDebug() << "****************************************";
    qDebug() << "*****        QML errors:           *****";
    qDebug() << "****************************************";
    for ( const QQmlError &error : component.errors() )
    {
      qDebug() << "  " << error;
    }
    qDebug() << "****************************************";
    qDebug() << "****************************************";
  }

  if ( object == nullptr )
  {
    qDebug() << "FATAL ERROR: unable to create main.qml";
    return EXIT_FAILURE;
  }


#ifdef Q_OS_IOS
  QString logoUrl = "qrc:logo.png";
#else
  QString logoUrl = ":/logo.png";
#endif
  if ( QQuickWindow *quickWindow = qobject_cast<QQuickWindow *>( object ) )
  {
    quickWindow->setIcon( QIcon( logoUrl ) );
  }

#ifdef DESKTOP_OS
  QCommandLineParser parser;
  parser.addVersionOption();
  parser.process( app );
#endif

  // Add some data for debugging if needed (visible in the final customer app)
  QgsApplication::messageLog()->logMessage( QgsQuickUtils().dumpScreenInfo() );
  QgsApplication::messageLog()->logMessage( "data directory: " + dataDir );
  QgsApplication::messageLog()->logMessage( "All up and running" );

#ifdef ANDROID
  QtAndroid::hideSplashScreen();
#endif

  return app.exec();
}


