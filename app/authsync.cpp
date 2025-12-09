#include "authsync.h"
#include <QFile>
#include <QRegularExpression>
#include <QMessageBox>
#include <QCryptographicHash>

const QString AUTH_CONFIG_FILENAME = "qgis_cfg.xml";

AuthSync::AuthSync( const QString &projectDir, QObject *parent )
  : QObject( parent )
  , mProject( QgsProject::instance() )
  , mProjectDir( projectDir )
  , mAuthMngr( QgsApplication::authManager() )
{
  mAuthFile = QDir( mProjectDir ).filePath( AUTH_CONFIG_FILENAME );
}

QString AuthSync::getProjectUuid( const QString &projectDir ) const
{
  return MerginProjectMetadata::fromCachedJson( CoreUtils::getProjectMetadataPath( projectDir ) ).projectId;
}

void AuthSync::importAuth()
{
  QFile authFile( mAuthFile );

  if ( authFile.exists() && QFileInfo::exists( mAuthFile ) )
  {
    if ( !mAuthMngr->masterPasswordIsSet() )
    {
      bool isSet = mAuthMngr->setMasterPassword( true );
      if ( !isSet )
      {
        CoreUtils::log("AuthSync manager", "Master password is not set. Could not import auth config." );
        QString userMsg = "Could not import authentication configuration for the protected layer(s). Set the master password and reload the project if you want to access the protected layer(s).";
        QMessageBox::warning( nullptr, "Could not load protected layer", userMsg, QMessageBox::Close );

      }
    }

    QString projectId = getProjectUuid( mProjectDir );

    bool ok = mAuthMngr->importAuthenticationConfigsFromXml( mAuthFile, projectId, true );
    CoreUtils::log( "AuthSync manager", QString( "QGIS auth imported: %1" ).arg( ok ? "true" : "false" ) );
  }
}