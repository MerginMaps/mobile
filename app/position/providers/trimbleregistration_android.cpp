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

static constexpr char TMM_EXTRA_APP_ID[] = "applicationID";
static constexpr char TMM_EXTRA_RESULT[] = "registrationResult";
static constexpr char TMM_EXTRA_LOCATION_V2_PORT[] = "locationV2Port";

static constexpr char TMM_RESULT_OK[] = "OK";

// Concrete subclass of the abstract QAndroidActivityResultReceiver.
// Forwards the result to TrimbleRegistration via a stored pointer.
class TmmResultReceiver : public QAndroidActivityResultReceiver
{
  public:
    explicit TmmResultReceiver( TrimbleRegistration *reg ) : mReg( reg ) {}

    void handleActivityResult( int receiverRequestCode, int resultCode, const QJniObject &data ) override
    {
      Q_UNUSED( resultCode )
      if ( receiverRequestCode != TMM_REGISTER_REQUEST_CODE || !mReg )
        return;

      if ( !data.isValid() )
      {
        emit mReg->failed( TrimbleRegistration::tr( "No response from Trimble Mobile Manager" ) );
        return;
      }

      QJniObject resultKey = QJniObject::fromString( QString::fromLatin1( TMM_EXTRA_RESULT ) );
      QJniObject resultObj = data.callObjectMethod( "getStringExtra",
                             "(Ljava/lang/String;)Ljava/lang/String;",
                             resultKey.object<jstring>() );
      const QString result = resultObj.isValid() ? resultObj.toString() : QString();

      if ( result != QLatin1String( TMM_RESULT_OK ) )
      {
        emit mReg->failed( TrimbleRegistration::tr( "Trimble Mobile Manager registration failed: %1" ).arg( result ) );
        return;
      }

      QJniObject portKey = QJniObject::fromString( QString::fromLatin1( TMM_EXTRA_LOCATION_V2_PORT ) );
      const int port = data.callMethod<jint>( "getIntExtra",
                                              "(Ljava/lang/String;I)I",
                                              portKey.object<jstring>(),
                                              static_cast<jint>( 0 ) );
      if ( port <= 0 )
      {
        emit mReg->failed( TrimbleRegistration::tr( "Trimble Mobile Manager returned invalid port" ) );
        return;
      }

      emit mReg->registered( port );
    }

  private:
    TrimbleRegistration *mReg = nullptr;
};

TrimbleRegistration::TrimbleRegistration( QObject *parent )
  : QObject( parent )
  , mResultReceiver( std::make_unique<TmmResultReceiver>( this ) )
{
}

TrimbleRegistration::~TrimbleRegistration() = default;

void TrimbleRegistration::requestRegistration( const QString &appId )
{
  QJniObject intentAction = QJniObject::fromString( QString::fromLatin1( TMM_ACTION_REGISTER ) );
  QJniObject intent( "android/content/Intent", "(Ljava/lang/String;)V", intentAction.object<jstring>() );
  QJniObject appIdStr = QJniObject::fromString( appId );
  QJniObject extraKey = QJniObject::fromString( QString::fromLatin1( TMM_EXTRA_APP_ID ) );
  intent.callObjectMethod( "putExtra",
                           "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
                           extraKey.object<jstring>(),
                           appIdStr.object<jstring>() );

  QtAndroidPrivate::startActivity( intent, TMM_REGISTER_REQUEST_CODE, mResultReceiver.get() );
}

void TrimbleRegistration::handleCallback( const QUrl &url )
{
  Q_UNUSED( url )
  // Android registration is intent-based; this callback is iOS-only
}

#endif // ANDROID
