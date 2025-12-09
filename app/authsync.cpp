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

void AuthSync::logError( const QString &message ) const
{
  CoreUtils::log( "AuthSync", "ERROR: " + message );
}

void AuthSync::logInfo( const QString &message ) const
{
  CoreUtils::log( "AuthSync", "INFO: " + message );
}

void AuthSync::logWarning( const QString &message ) const
{
  CoreUtils::log( "AuthSync", "WARNING: " + message );
}


QStringList AuthSync::getLayersAuthIds() const
{
  QStringList authIds;
  QgsProviderRegistry *reg = QgsProviderRegistry::instance();

  for ( QgsMapLayer *layer : mProject->mapLayers().values() )
  {
    QString source = layer->source();
    QString provType = layer->providerType();
    QVariantMap decodedUri = reg->decodeUri( provType, source );

    QString authId = decodedUri.value( "authcfg" ).toString();
    if ( !authId.isEmpty() )
    {
      authIds += authId ;
    }
  }
  return authIds;
}

QString AuthSync::getAuthConfigHash( const QStringList &authIds ) const
{
  QStringList sortedIds = authIds;
  std::sort( sortedIds.begin(), sortedIds.end() );

  QCryptographicHash hasher( QCryptographicHash::Sha256 );

  for ( const QString &authId : sortedIds )
  {
    QgsAuthMethodConfig config;
    // True to decrypt full details
    if ( !mAuthMngr->loadAuthenticationConfig( authId, config, true ) )
    {
      logError( QString( "Failed to load the authentication config for the auth ID: %1" ).arg( authId ) );
      continue;
    }

    QString headerData = QString( "%1|%2|%3" ).arg( config.id(), config.method(), config.uri() );
    hasher.addData( headerData.toUtf8() );

    QMap<QString, QString> configMap = config.configMap();
    QStringList sortedKeys = configMap.keys();
    std::sort( sortedKeys.begin(), sortedKeys.end() );

    for ( const QString &key : sortedKeys )
    {
      QString entry = QString( "|%1=%2" ).arg( key, configMap.value( key ) );
      hasher.addData( entry.toUtf8() );
    }
  }

  return QString( hasher.result().toHex() );
}

void AuthSync::exportAuth( const LocalProject &localProject )
{
  QStringList authIds = getLayersAuthIds();
  if ( authIds.isEmpty() )
    return;

  if ( !mAuthMngr->masterPasswordIsSet() )
  {
    logWarning( "Master Password not set. Cannot export auth configs." );
    QString msg = "Failed to export authentication configuration. If you want to share the credentials of the protected layer(s), set the master password please.";
    QMessageBox::warning(
      nullptr, "Cannot export configuration for protected layer", msg, QMessageBox::Close
    );
    return;
  }

  QString currentHash = getAuthConfigHash( authIds );
  QFile authFile( mAuthFile );
  bool fileExists = authFile.exists();
  bool shouldExport = true;

  if ( fileExists )
  {
    if ( authFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
      QString content = authFile.readAll();
      authFile.close();

      // Regex to find the HASH comment
      QRegularExpression pattern( "" );
      QRegularExpressionMatch match = pattern.match( content );

      if ( match.hasMatch() )
      {
        QString existingHash = match.captured( 1 );
        if ( existingHash == currentHash )
        {
          logInfo( "No change in auth config. No update needed." );
          shouldExport = false;
        }
        else
        {
          logInfo( "Auth config file change detected. Updating file..." );
        }
      }
      else
      {
        logWarning( "No hash found in existing config file. Creating one..." );
      }
    }
    else
    {
      logError( "Failed to open existing auth config file for reading." );
    }
  }

  if ( !shouldExport )
    return;

  QString tempFile = QDir( mProjectDir ).filePath( QString( "temp_%1" ).arg( AUTH_CONFIG_FILENAME ) );
  QString projectId = getProjectUuid( mProjectDir );

  bool success = mAuthMngr->exportAuthenticationConfigsToXml( tempFile, authIds, projectId );

  if ( success )
  {
    QFile temp( tempFile );
    if ( temp.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
      QString xmlContent = temp.readAll();
      temp.close();

      QString hashedContent = xmlContent + QString( "\n" ).arg( currentHash );

      if ( authFile.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate ) )
      {
        authFile.write( hashedContent.toUtf8() );
        authFile.close();
      }
      else
      {
        logError( QString( "Failed to write exported hash content to file: %1" ).arg( mAuthFile ) );
      }
    }
    else
    {
      logError( QString( "Failed to read content from temporary export file: %1" ).arg( tempFile ) );
    }

    QFile::remove( tempFile );
  }
  else
  {
    logError( QString( "Failed to export authentication configurations to XML: %1" ).arg( tempFile ) );
  }
}


void AuthSync::importAuth()
{
  QFile authFile( mAuthFile );

  if ( authFile.exists() && QFileInfo::exists( mAuthFile ) )
  {
    if ( !mAuthMngr->masterPasswordIsSet() )
    {
       bool isSet = mAuthMngr->setMasterPassword(true);
       if(!isSet)
      {
         logWarning( "Master password is not set. Could not import auth config." );
       QString userMsg = "Could not import authentication configuration for the protected layer(s). Set the master password and reload the project if you want to access the protected layer(s).";
       QMessageBox::warning( nullptr, "Could not load protected layer", userMsg, QMessageBox::Close );
      
       }
    }

    QString projectId = getProjectUuid( mProjectDir );

    bool ok = mAuthMngr->importAuthenticationConfigsFromXml( mAuthFile, projectId, true );
    logInfo( QString( "QGIS auth imported: %1" ).arg( ok ? "true" : "false" ) );
  }
}