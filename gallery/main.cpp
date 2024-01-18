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

int main( int argc, char *argv[] )
{
  QGuiApplication app( argc, argv );

  app.setFont( QFont( Helper::installFonts() ) );

  QQmlApplicationEngine engine;

  // Register C++ enums
  qmlRegisterUncreatableType<RegistrationError>( "lc", 1, 0, "RegistrationError", "RegistrationError Enum" );
  qmlRegisterType<QrCodeDecoder>( "lc", 1, 0, "QrCodeDecoder" );

#ifdef DESKTOP_OS
  HotReload hotReload( engine );
  engine.rootContext()->setContextProperty( "_hotReload", &hotReload );
#endif
  InputUtils iu;
  engine.rootContext()->setContextProperty( "__inputUtils", &iu );

  qreal dp = Helper::calculateDpRatio();
  MMStyle style( dp );
  NotificationModel notificationModel;

  engine.rootContext()->setContextProperty( "__notificationModel", &notificationModel );

  // path to local wrapper pages
  engine.rootContext()->setContextProperty( "_qmlWrapperPath", QGuiApplication::applicationDirPath() + "/HotReload/qml/pages/" );
  engine.rootContext()->setContextProperty( "__dp", dp );
  engine.rootContext()->setContextProperty( "__style", &style );
  engine.rootContext()->setContextProperty( "__isMobile", Helper::isMobile() );

  QObject::connect( &engine, &QQmlApplicationEngine::objectCreationFailed,
  &app, []() { QCoreApplication::exit( -1 ); }, Qt::QueuedConnection );
  engine.loadFromModule( "gallery", "Main" );

  return app.exec();
}
