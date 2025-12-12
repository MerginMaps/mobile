#include "authsync.h"
#include <QFile>

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

bool AuthSync::fileExists( QString path )
{
  QFileInfo check_file( path );
  // check if path exists and it is indeed a file
  if ( check_file.exists() || check_file.isFile() )
  {
    return check_file.isWritable();
  }
  else
  {
    return false;
  }
}

void AuthSync::importAuth()
{
  if ( fileExists( mAuthFile ) )
  {
    QString projectId = getProjectUuid( mProjectDir );

    bool ok = mAuthMngr->importAuthenticationConfigsFromXml( mAuthFile, projectId, true );
    CoreUtils::log( "AuthSync manager", QString( "QGIS auth imported: %1" ).arg( ok ? "true" : "false" ) );
  }
}