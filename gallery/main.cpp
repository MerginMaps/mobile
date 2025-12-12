/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#ifdef DESKTOP_OS
#include "hotreload.h"
#endif
#include "mmstyle.h"
#include "helper.h"
#include <QFont>
#include <QFontDatabase>
#include "notificationmodel.h"
#include "merginerrortypes.h"
#include "qrcodedecoder.h"
#include "inpututils.h"
#include "scalebarkit.h"
#include "formfeaturesmodel.h"
#include "enums.h"

int main( int argc, char *argv[] )
{
  QGuiApplication app( argc, argv );

  // These must be set so that QSettings work properly
  QCoreApplication::setOrganizationName( "Lutra Consulting" );
  QCoreApplication::setOrganizationDomain( "lutraconsulting.co.uk" );
  QCoreApplication::setApplicationName( "Mobile gallery" ); // used by QSettings
  QCoreApplication::setApplicationVersion( "0.1" );

  app.setFont( QFont( Helper::installFonts() ) );

  InputUtils iu;

  QQmlApplicationEngine engine;

  // Register C++ enums
  qmlRegisterUncreatableType<RegistrationError>( "mm", 1, 0, "RegistrationError", "RegistrationError Enum" );
  qmlRegisterUncreatableType<StreamingIntervalType>( "mm", 1, 0, "StreamingIntervalType", "StreamingIntervalType Enum" );
  qmlRegisterUncreatableType<PositionProviderType>( "mm", 1, 0, "PositionProvider", "PositionProvider Enum" );
  qmlRegisterUncreatableType<NotificationType>( "mm", 1, 0, "NotificationType", "NotificationType Enum" );
  qmlRegisterUncreatableType<ProjectStatus>( "mm", 1, 0, "ProjectStatus", "ProjectStatus Enum" );

  // Register C++ types
  qmlRegisterType<QrCodeDecoder>( "mm", 1, 0, "QrCodeDecoder" );
  qmlRegisterType<ScaleBarKit>( "mm", 1, 0, "ScaleBarKit" );
  qmlRegisterType<FormFeaturesModel>( "mm", 1, 0, "FeaturesModel" );
  qmlRegisterType<FormFeaturesModel>( "mm", 1, 0, "RelationFeaturesModel" );
  qmlRegisterType<FormFeaturesModel>( "mm", 1, 0, "RelationReferenceFeaturesModel" );


#ifdef DESKTOP_OS
  HotReload hotReload( engine );
  engine.rootContext()->setContextProperty( "_hotReload", &hotReload );
#endif

  engine.rootContext()->setContextProperty( "__inputUtils", &iu );
  engine.rootContext()->setContextProperty( "__androidUtils", &iu );
  engine.rootContext()->setContextProperty( "__iosUtils", &iu );

  qreal dp = Helper::calculateDpRatio();

  // MMStyle must be destructed after engine
  // see https://tobiasmarciszko.github.io/qml-binding-errors/
  MMStyle *style = new MMStyle( &engine, dp );

  // Set some safe areas
  style->setSafeAreaTop( 25 );
  style->setSafeAreaRight( 10 );
  style->setSafeAreaBottom( 10 );
  style->setSafeAreaLeft( 10 );

  NotificationModel notificationModel;

  engine.rootContext()->setContextProperty( "__notificationModel", &notificationModel );
  // path to local wrapper pages
  engine.rootContext()->setContextProperty( "_qmlWrapperPath", QGuiApplication::applicationDirPath() + "/HotReload/qml/pages/" );
  engine.rootContext()->setContextProperty( "__dp", dp );
  engine.rootContext()->setContextProperty( "__style", style );
  engine.rootContext()->setContextProperty( "__isMobile", Helper::isMobile() );
  engine.rootContext()->setContextProperty( "__logText", Helper::logText() );
  engine.rootContext()->setContextProperty( "__version", "2222.1.12" );

  QObject::connect( &engine, &QQmlApplicationEngine::objectCreationFailed,
  &app, []() { QCoreApplication::exit( -1 ); }, Qt::QueuedConnection );
  engine.loadFromModule( "gallery", "Main" );

  return app.exec();
}
