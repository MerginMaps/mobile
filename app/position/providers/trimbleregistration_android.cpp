/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "trimbleregistration.h"

#ifdef ANDROID

#include <QJniObject>
#include <QJniEnvironment>
#include <QtCore/private/qandroidextras_p.h>

static constexpr int TMM_REGISTER_REQUEST_CODE = 0x544D4D52; // "TMMR"

static constexpr char TMM_ACTION_REGISTER[] = "com.trimble.tmm.REGISTER";
static constexpr char TMM_ACTION_OPEN_ANTENNA_HEIGHT[] = "com.trimble.tmm.OPENANTENNAHEIGHT";

static constexpr char TMM_EXTRA_APP_ID[] = "applicationID";
static constexpr char TMM_EXTRA_RESULT[] = "registrationResult";
static constexpr char TMM_EXTRA_LOCATION_V2_PORT[] = "locationV2Port";

static constexpr char TMM_RESULT_OK[] = "OK";

TrimbleRegistration::TrimbleRegistration( QObject *parent )
  : QObject( parent )
{
}

void TrimbleRegistration::requestRegistration( const QString &appId )
{
  QJniObject intentAction = QJniObject::fromString( TMM_ACTION_REGISTER );
  QJniObject intent( "android/content/Intent", "(Ljava/lang/String;)V", intentAction.object<jstring>() );
  QJniObject appIdStr = QJniObject::fromString( appId );
  QJniObject extraKey = QJniObject::fromString( TMM_EXTRA_APP_ID );
  intent.callObjectMethod( "putExtra",
                           "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
                           extraKey.object<jstring>(),
                           appIdStr.object<jstring>() );

  auto resultReceiver = std::make_shared<QAndroidActivityResultReceiver>();
  QObject::connect( resultReceiver.get(), &QAndroidActivityResultReceiver::resultReceived,
                    this, [this, resultReceiver]( int requestCode, int resultCode, const QJniObject & data )
  {
    Q_UNUSED( resultCode )
    if ( requestCode != TMM_REGISTER_REQUEST_CODE )
      return;

    if ( !data.isValid() )
    {
      emit failed( tr( "No response from Trimble Mobile Manager" ) );
      return;
    }

    QJniObject resultKey = QJniObject::fromString( TMM_EXTRA_RESULT );
    QJniObject resultObj = data.callObjectMethod( "getStringExtra",
                           "(Ljava/lang/String;)Ljava/lang/String;",
                           resultKey.object<jstring>() );
    const QString result = resultObj.isValid() ? resultObj.toString() : QString();

    if ( result != QLatin1String( TMM_RESULT_OK ) )
    {
      emit failed( tr( "Trimble Mobile Manager registration failed: %1" ).arg( result ) );
      return;
    }

    QJniObject portKey = QJniObject::fromString( TMM_EXTRA_LOCATION_V2_PORT );
    const int port = data.callMethod<jint>( "getIntExtra",
                                            "(Ljava/lang/String;I)I",
                                            portKey.object<jstring>(),
                                            static_cast<jint>( 0 ) );
    if ( port <= 0 )
    {
      emit failed( tr( "Trimble Mobile Manager returned invalid port" ) );
      return;
    }

    emit registered( port );
  } );

  QtAndroidPrivate::startActivity( intent, TMM_REGISTER_REQUEST_CODE, resultReceiver.get() );
}

Q_INVOKABLE void openTrimbleAntennaHeightPage()
{
  QJniObject intentAction = QJniObject::fromString( TMM_ACTION_OPEN_ANTENNA_HEIGHT );
  QJniObject intent( "android/content/Intent", "(Ljava/lang/String;)V", intentAction.object<jstring>() );

  QJniObject activity = QJniObject::callStaticObjectMethod( "org/qtproject/qt/android/QtNative",
                        "activity",
                        "()Landroid/app/Activity;" );
  if ( activity.isValid() )
  {
    activity.callMethod<void>( "startActivity", "(Landroid/content/Intent;)V", intent.object() );
  }
}

#endif // ANDROID
