#include "merginapi.h"

#include <QtNetwork>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>
#include <QByteArray>
#include <QSet>
#include <QMessageBox>
#include <QUuid>
#include <QtMath>

#include "inpututils.h"

const QString MerginApi::sMetadataFile = QStringLiteral( "/.mergin/mergin.json" );

MerginApi::MerginApi( const QString &dataDir, QObject *parent )
  : QObject( parent )
  , mDataDir( dataDir )
{
  QObject::connect( this, &MerginApi::syncProjectFinished, this, &MerginApi::updateProjectMetadata );
  QObject::connect( this, &MerginApi::authChanged, this, &MerginApi::saveAuthData );
  QObject::connect( this, &MerginApi::serverProjectDeleted, this, &MerginApi::projectDeleted );
  QObject::connect( this, &MerginApi::apiRootChanged, this, &MerginApi::pingMergin );
  QObject::connect( this, &MerginApi::pingMerginFinished, this, &MerginApi::checkMerginVersion );
  QObject::connect( this, &MerginApi::downloadFileFinished, this, &MerginApi::continueDownloadFiles );

  loadAuthData();
  mMerginProjects = parseAllProjectsMetadata();
}

void MerginApi::listProjects( const QString &searchExpression, const QString &user,
                              const QString &flag, const QString &filterTag )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QNetworkRequest request;
  // projects filtered by tag "input_use"
  QString urlString = mApiRoot + QStringLiteral( "/v1/project" );
  if ( !filterTag.isEmpty() )
  {
    urlString += QStringLiteral( "?tags=" ) + filterTag;
  }
  if ( !searchExpression.isEmpty() )
  {
    urlString += QStringLiteral( "&q=" ) + searchExpression;
  }
  if ( !flag.isEmpty() )
  {
    urlString += QStringLiteral( "&flag=%1&user=%2" ).arg( flag ).arg( user );
  }
  QUrl url( urlString );
  request.setUrl( url );
  request.setRawHeader( "Authorization", QByteArray( "Bearer " + mAuthToken ) );

  QNetworkReply *reply = mManager.get( request );
  InputUtils::log( url.toString(), QStringLiteral( "STARTED" ) );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::listProjectsReplyFinished );
}

void MerginApi::downloadFile( const QString &projectFullName, const QString &filename, const QString &version, int chunkNo )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QNetworkRequest request;
  QUrl url( mApiRoot + QStringLiteral( "/v1/project/raw/%1?file=%2&version=%3" ).arg( projectFullName ).arg( filename ).arg( version ) );
  request.setUrl( url );
  request.setRawHeader( "Authorization", QByteArray( "Bearer " + mAuthToken ) );
  request.setAttribute( QNetworkRequest::User, QVariant( chunkNo ) );

  QString range;
  int from = UPLOAD_CHUNK_SIZE * chunkNo;
  int to = UPLOAD_CHUNK_SIZE * ( chunkNo + 1 ) - 1;
  range = QStringLiteral( "bytes=%1-%2" ).arg( from ).arg( to );
  request.setRawHeader( "Range", range.toUtf8() );

  mPendingRequests.insert( url, projectFullName );
  QNetworkReply *reply = mManager.get( request );
  InputUtils::log( url.toString() + " Range: " + range, QStringLiteral( "STARTED" ) );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::downloadFileReplyFinished );
}

void MerginApi::uploadFile( const QString &projectFullName, const QString &transactionUUID, MerginFile file, int chunkNo )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QString projectNamespace;
  QString projectName;
  extractProjectName( projectFullName, projectNamespace, projectName );
  QString projectDir = getProjectDir( projectNamespace, projectName );
  QString chunkID = file.chunks.at( chunkNo );

  QFile f( projectDir + "/" + file.path );
  QByteArray data;

  if ( f.open( QIODevice::ReadOnly ) )
  {
    f.seek( chunkNo * UPLOAD_CHUNK_SIZE );
    data = f.read( UPLOAD_CHUNK_SIZE );
  }

  QNetworkRequest request;
  QUrl url( mApiRoot + QStringLiteral( "/v1/project/push/chunk/%1/%2" ).arg( transactionUUID ).arg( chunkID ) );
  request.setUrl( url );
  request.setRawHeader( "Authorization", QByteArray( "Bearer " + mAuthToken ) );
  request.setRawHeader( "Content-Type", "application/octet-stream" );

  QNetworkReply *reply = mManager.post( request, data );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::uploadFileReplyFinished );
  mPendingRequests.insert( url, projectFullName );
  mOpenConnections.insert( projectFullName, reply );
  InputUtils::log( url.toString(), QStringLiteral( "STARTED" ) );
}

void MerginApi::uploadStart( const QString &projectFullName, const QByteArray &json )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  // Has been canceled
  if ( !mTransactions.contains( projectFullName ) )
  {
    InputUtils::log( "uploadStarted", QStringLiteral( "ABORT" ) );
    return;
  }

  QNetworkRequest request;
  QUrl url( mApiRoot + QStringLiteral( "v1/project/push/%1" ).arg( projectFullName ) );
  request.setUrl( url );
  request.setRawHeader( "Authorization", QByteArray( "Bearer " + mAuthToken ) );
  request.setRawHeader( "Content-Type", "application/json" );

  QNetworkReply *reply = mManager.post( request, json );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::uploadStartReplyFinished );
  mPendingRequests.insert( url, projectFullName );
  mOpenConnections.insert( projectFullName, reply );
  InputUtils::log( url.toString(), QStringLiteral( "STARTED" ) );
}

void MerginApi::uploadCancel( const QString &projectFullName )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QNetworkReply *reply = mOpenConnections.value( projectFullName );
  if ( reply )
  {
    InputUtils::log( reply->url().toString(), QStringLiteral( "ABORT" ) );
    reply->abort();
  }
  mOpenConnections.remove( projectFullName );

  QString transactionUUID = mTransactions.value( projectFullName );

  // Transaction has not started yet
  if ( transactionUUID == projectFullName )
  {
    mTransactions.remove( projectFullName );
    emit syncProjectFinished( QStringLiteral(), projectFullName, false );
  }
  else
  {
    QNetworkRequest request;
    QUrl url( mApiRoot + QStringLiteral( "v1/project/push/cancel/%1" ).arg( transactionUUID ) );
    request.setUrl( url );
    request.setRawHeader( "Authorization", QByteArray( "Bearer " + mAuthToken ) );
    request.setRawHeader( "Content-Type", "application/json" );
    mPendingRequests.insert( url, projectFullName );

    QNetworkReply *reply = mManager.post( request, QByteArray() );
    connect( reply, &QNetworkReply::finished, this, &MerginApi::uploadCancelReplyFinished );
    InputUtils::log( url.toString(), QStringLiteral( "STARTED" ) );
  }
}

void MerginApi::uploadFinish( const QString &projectFullName, const QString &transactionUUID )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  if ( !mTransactions.contains( projectFullName ) )
  {
    return;
  }

  QNetworkRequest request;
  QUrl url( mApiRoot + QStringLiteral( "v1/project/push/finish/%1" ).arg( transactionUUID ) );
  request.setUrl( url );
  request.setRawHeader( "Authorization", QByteArray( "Bearer " + mAuthToken ) );
  request.setRawHeader( "Content-Type", "application/json" );

  QNetworkReply *reply = mManager.post( request, QByteArray() );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::uploadFinishReplyFinished );
  mPendingRequests.insert( url, projectFullName );
  mOpenConnections.insert( projectFullName, reply );
  InputUtils::log( url.toString(), QStringLiteral( "STARTED" ) );
}

void MerginApi::updateProject( const QString &projectNamespace, const QString &projectName )
{
  QString projectFullName = getFullProjectName( projectNamespace, projectName );
  QNetworkReply *reply = getProjectInfo( projectFullName );
  if ( reply )
    connect( reply, &QNetworkReply::finished, this, &MerginApi::updateInfoReplyFinished );
}

void MerginApi::uploadProject( const QString &projectNamespace, const QString &projectName )
{
  bool onlyUpload = true;
  QString projectFullName = getFullProjectName( projectNamespace, projectName );
  QString projectDir = getProjectDir( projectNamespace, projectName );

  // Waiting for transaction UUID, just added projectFullName as a tag its pending
  mTransactions.insert( projectFullName, projectFullName );
  for ( std::shared_ptr<MerginProject> project : mMerginProjects )
  {
    if ( getFullProjectName( project->projectNamespace, project->name ) == projectFullName )
    {
      if ( project->clientUpdated < project->serverUpdated && project->serverUpdated > project->lastSyncClient.toUTC() )
      {
        onlyUpload = false;
      }
    }
  }

  if ( onlyUpload )
  {
    continueWithUpload( projectDir, projectFullName, true );
  }
  else
  {
    QMessageBox msgBox;
    msgBox.setText( QStringLiteral( "The project has been updated on the server in the meantime. Your files will be updated before upload." ) );
    msgBox.setInformativeText( "Do you want to continue?" );
    msgBox.setStandardButtons( QMessageBox::Ok | QMessageBox::Cancel );
    msgBox.setDefaultButton( QMessageBox::Cancel );

    if ( msgBox.exec() == QMessageBox::Cancel )
    {
      emit syncProjectFinished( projectDir, projectFullName, false );
      return;
    }

    mWaitingForUpload.insert( projectFullName );
    updateProject( projectNamespace, projectName );
    connect( this, &MerginApi::syncProjectFinished, this, &MerginApi::continueWithUpload );
  }
}

void MerginApi::authorize( const QString &username, const QString &password )
{
  if ( username.contains( "@" ) )
  {
    mUsername = username.split( "@" ).first();
  }
  else
  {
    mUsername = username;
  }
  mPassword = password;

  QNetworkRequest request;
  QString urlString = mApiRoot + QStringLiteral( "v1/auth/login" );
  QUrl url( urlString );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );

  QJsonDocument jsonDoc;
  QJsonObject jsonObject;
  jsonObject.insert( QStringLiteral( "login" ), mUsername );
  jsonObject.insert( QStringLiteral( "password" ), mPassword );
  jsonDoc.setObject( jsonObject );
  QByteArray json = jsonDoc.toJson( QJsonDocument::Compact );

  QNetworkReply *reply = mManager.post( request, json );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::authorizeFinished );
}

void MerginApi::getUserInfo( const QString &username )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QNetworkRequest request;
  QString urlString = mApiRoot + QStringLiteral( "v1/user/" ) + username;
  QUrl url( urlString );
  request.setUrl( url );
  request.setRawHeader( "Authorization", QByteArray( "Bearer " + mAuthToken ) );

  QNetworkReply *reply = mManager.get( request );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::getUserInfoFinished );
}

void MerginApi::clearAuth()
{
  mUsername = "";
  mPassword = "";
  mAuthToken.clear();
  mTokenExpiration.setTime( QTime() );
  mUserId = -1;
  mDiskUsage = 0;
  mStorageLimit = 0;
  emit authChanged();
}

void MerginApi::resetApiRoot()
{
  QSettings settings;
  settings.beginGroup( QStringLiteral( "Input/" ) );
  setApiRoot( defaultApiRoot() );
  settings.endGroup();
}

bool MerginApi::hasAuthData()
{
  return !mUsername.isEmpty() && !mPassword.isEmpty();
}

void MerginApi::createProject( const QString &projectNamespace, const QString &projectName )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QNetworkRequest request;
  QUrl url( mApiRoot + QString( "/v1/project/%1" ).arg( projectNamespace ) );
  request.setUrl( url );
  request.setRawHeader( "Authorization", QByteArray( "Bearer " + mAuthToken ) );
  request.setRawHeader( "Content-Type", "application/json" );
  request.setRawHeader( "Accept", "application/json" );
  mPendingRequests.insert( url, getFullProjectName( projectNamespace, projectName ) );

  QJsonDocument jsonDoc;
  QJsonObject jsonObject;
  jsonObject.insert( QStringLiteral( "name" ), projectName );
  jsonObject.insert( QStringLiteral( "public" ), false );
  jsonDoc.setObject( jsonObject );
  QByteArray json = jsonDoc.toJson( QJsonDocument::Compact );

  QNetworkReply *reply = mManager.post( request, json );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::createProjectFinished );
  InputUtils::log( url.toString(), QStringLiteral( "STARTED" ) );
}

void MerginApi::deleteProject( const QString &projectNamespace, const QString &projectName )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QNetworkRequest request;
  QUrl url( mApiRoot + QStringLiteral( "/v1/project/%1/%2" ).arg( projectNamespace ).arg( projectName ) );
  request.setUrl( url );
  request.setRawHeader( "Authorization", QByteArray( "Bearer " + mAuthToken ) );
  mPendingRequests.insert( url, getFullProjectName( projectNamespace, projectName ) );
  QNetworkReply *reply = mManager.deleteResource( request );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::deleteProjectFinished );
  InputUtils::log( url.toString(), QStringLiteral( "STARTED" ) );
}

void MerginApi::clearTokenData()
{
  mTokenExpiration = QDateTime().currentDateTime().addDays( -42 ); // to make it expired arbitrary days ago
  mAuthToken.clear();
}

ProjectList MerginApi::updateMerginProjectList( const ProjectList &serverProjects )
{
  QHash<QString, std::shared_ptr<MerginProject>> downloadedProjects;
  for ( std::shared_ptr<MerginProject> project : mMerginProjects )
  {
    if ( !project->projectDir.isEmpty() )
    {
      downloadedProjects.insert( getFullProjectName( project->projectNamespace, project->name ), project );
    }
  }

  if ( downloadedProjects.isEmpty() ) return serverProjects;

  for ( std::shared_ptr<MerginProject> project : serverProjects )
  {
    QString fullProjectName = getFullProjectName( project->projectNamespace, project->name );
    if ( downloadedProjects.contains( fullProjectName ) )
    {
      project->projectDir = downloadedProjects.value( fullProjectName ).get()->projectDir;
      QDateTime localUpdate = downloadedProjects.value( fullProjectName ).get()->clientUpdated.toUTC();
      project->lastSyncClient = downloadedProjects.value( fullProjectName ).get()->lastSyncClient.toUTC();
      QDateTime lastModified = getLastModifiedFileDateTime( project->projectDir );
      project->clientUpdated = localUpdate;
      project->status = getProjectStatus( project, lastModified );
    }
  }
  return serverProjects;
}

void MerginApi::deleteObsoleteFiles( const QString &projectPath )
{
  if ( !mObsoleteFiles.value( projectPath ).isEmpty() )
  {
    for ( QString filename : mObsoleteFiles.value( projectPath ) )
    {
      QFile file( projectPath + filename );
      file.remove();
    }
    mObsoleteFiles.remove( projectPath );
  }
}

void MerginApi::saveAuthData()
{
  QSettings settings;
  settings.beginGroup( "Input/" );
  settings.setValue( "username", mUsername );
  settings.setValue( "password", mPassword );
  settings.setValue( "userId", mUserId );
  settings.setValue( "token", mAuthToken );
  settings.setValue( "expire", mTokenExpiration );
  settings.setValue( "apiRoot", mApiRoot );
  settings.endGroup();
}

void MerginApi::createProjectFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    QString projectFullName = mPendingRequests.value( r->url() );
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
    emit notify( QStringLiteral( "Project created" ) );
    emit projectCreated( projectFullName );
  }
  else
  {
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( r->errorString() ) );
    emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: createProject" ) );
  }
  mPendingRequests.remove( r->url() );
  r->deleteLater();
}

void MerginApi::deleteProjectFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    QString projectFullName = mPendingRequests.value( r->url() );
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
    emit notify( QStringLiteral( "Project deleted" ) );
    emit serverProjectDeleted( projectFullName );
  }
  else
  {
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( r->errorString() ) );
    emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: deleteProject" ) );
  }
  mPendingRequests.remove( r->url() );
  r->deleteLater();
}

void MerginApi::authorizeFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
    QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      QJsonObject docObj = doc.object();
      QJsonObject session = docObj.value( QStringLiteral( "session" ) ).toObject();
      mAuthToken = session.value( QStringLiteral( "token" ) ).toString().toUtf8();
      mTokenExpiration = QDateTime::fromString( session.value( QStringLiteral( "expire" ) ).toString(), Qt::ISODateWithMs ).toUTC();
      mUserId = docObj.value( QStringLiteral( "id" ) ).toInt();
      mDiskUsage = docObj.value( QStringLiteral( "disk_usage" ) ).toInt();
      mStorageLimit = docObj.value( QStringLiteral( "storage_limit" ) ).toInt();
    }
    emit authChanged();
  }
  else
  {
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( r->errorString() ) );
    QVariant statusCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    int status = statusCode.toInt();
    if ( status == 401 || status == 400 )
    {
      emit authFailed();
      emit notify( QStringLiteral( "Authentication failed" ) );
    }
    else
    {
      emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: authorize" ) );
    }
    mUsername.clear();
    mPassword.clear();
    clearTokenData();
  }
  if ( mAuthLoopEvent.isRunning() )
  {
    mAuthLoopEvent.exit();
  }
  r->deleteLater();
}

void MerginApi::pingMerginReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );
  QString apiVersion;
  QString msg;

  if ( r->error() == QNetworkReply::NoError )
  {
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
    QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      QJsonObject obj = doc.object();
      apiVersion = obj.value( QStringLiteral( "version" ) ).toString();
    }
  }
  else
  {
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( r->errorString() ) );
  }
  r->deleteLater();
  emit pingMerginFinished( apiVersion, msg );
}

ProjectList MerginApi::parseAllProjectsMetadata()
{
  QStringList entryList = QDir( mDataDir ).entryList( QDir::NoDotAndDotDot | QDir::Dirs );
  ProjectList projects;

  for ( QString folderName : entryList )
  {
    QFileInfo info( mDataDir + folderName + "/" + sMetadataFile );
    if ( info.exists() )
    {
      std::shared_ptr<MerginProject> project = readProjectMetadataFromPath( mDataDir + folderName );
      if ( project )
      {
        projects << project;
      }
    }
  }

  return projects;
}

void MerginApi::clearProject( std::shared_ptr<MerginProject> project )
{
  project->status = ProjectStatus::NoVersion;
  project->lastSyncClient = QDateTime();
  project->clientUpdated = QDateTime();
  project->serverUpdated = QDateTime();
  project->projectDir.clear();
  emit merginProjectsChanged();
}

QNetworkReply *MerginApi::getProjectInfo( const QString &projectFullName )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return nullptr;
  }

  QNetworkRequest request;
  QUrl url( mApiRoot + QStringLiteral( "/v1/project/%1" ).arg( projectFullName ) );

  request.setUrl( url );
  request.setRawHeader( "Authorization", QByteArray( "Bearer " + mAuthToken ) );

  mPendingRequests.insert( url, projectFullName );
  InputUtils::log( url.toString(), QStringLiteral( "STARTED" ) );
  return mManager.get( request );
}

void MerginApi::projectDeleted( const QString &projecFullName )
{
  std::shared_ptr<MerginProject> project = getProject( projecFullName );
  if ( project )
    clearProject( project );
}

void MerginApi::projectDeletedOnPath( const QString &projectDir )
{
  for ( std::shared_ptr<MerginProject> project : mMerginProjects )
  {
    if ( project->projectDir == mDataDir + projectDir )
    {
      clearProject( project );
    }
  }
}

void MerginApi::loadAuthData()
{
  QSettings settings;
  settings.beginGroup( QStringLiteral( "Input/" ) );
  setApiRoot( settings.value( QStringLiteral( "apiRoot" ) ).toString() );
  mUsername = settings.value( QStringLiteral( "username" ) ).toString();
  mPassword = settings.value( QStringLiteral( "password" ) ).toString();
  mUserId = settings.value( QStringLiteral( "userId" ) ).toInt();
  mTokenExpiration = settings.value( QStringLiteral( "expire" ) ).toDateTime();
  mAuthToken = settings.value( QStringLiteral( "token" ) ).toByteArray();
}

bool MerginApi::validateAuthAndContinute()
{
  if ( !hasAuthData() )
  {
    emit authRequested();
    return false;
  }

  if ( mAuthToken.isEmpty() || mTokenExpiration < QDateTime().currentDateTime().toUTC() )
  {
    authorize( mUsername, mPassword );

    mAuthLoopEvent.exec();
  }
  return true;
}

void MerginApi::checkMerginVersion( QString apiVersion, QString msg )
{
  if ( msg.isEmpty() )
  {
    int major = -1;
    int minor = -1;
    QRegularExpression re;
    re.setPattern( QStringLiteral( "(?<major>\\d+)[.](?<minor>\\d+)" ) );
    QRegularExpressionMatch match = re.match( apiVersion );
    if ( match.hasMatch() )
    {
      major = match.captured( "major" ).toInt();
      minor = match.captured( "minor" ).toInt();
    }

    if ( ( MERGIN_API_VERSION_MAJOR == major && MERGIN_API_VERSION_MINOR <= minor ) || ( MERGIN_API_VERSION_MAJOR < major ) )
    {
      setApiVersionStatus( MerginApiStatus::OK );
    }
    else
    {
      setApiVersionStatus( MerginApiStatus::INCOMPATIBLE );
    }
  }
  else
  {
    setApiVersionStatus( MerginApiStatus::NOT_FOUND );
  }
}

bool MerginApi::extractProjectName( const QString &sourceString, QString &projectNamespace, QString &name )
{
  QStringList parts = sourceString.split( "/" );
  if ( parts.length() > 1 )
  {
    projectNamespace = parts.at( parts.length() - 2 );
    name = parts.last();
    return true;
  }
  else
  {
    name = sourceString;
    return false;
  }
}

std::shared_ptr<MerginProject> MerginApi::getProject( const QString &projectFullName )
{
  for ( std::shared_ptr<MerginProject> project : mMerginProjects )
  {
    if ( projectFullName == getFullProjectName( project->projectNamespace, project->name ) )
    {
      return project;
    }
  }

  return std::shared_ptr<MerginProject>();
}

QString MerginApi::findUniqueProjectDirectoryName( QString path )
{
  QDir projectDir( path );
  if ( projectDir.exists() )
  {
    int i = 0;
    QFileInfo info( path + QString::number( i ) );
    while ( info.exists() && info.isDir() )
    {
      ++i;
      info.setFile( path + QString::number( i ) );
    }
    return path + QString::number( i );
  }
  else
  {
    return path;
  }
}

QString MerginApi::getProjectDir( const QString &projectNamespace, const QString &projectName )
{
  QString projectFullName = getFullProjectName( projectNamespace, projectName );
  std::shared_ptr<MerginProject> project = getProject( projectFullName );
  if ( project )
  {
    if ( project->projectDir.isEmpty() )
    {
      QString projectDirPath = findUniqueProjectDirectoryName( mDataDir + projectName );
      QDir projectDir( projectDirPath );
      if ( !projectDir.exists() )
      {
        QDir dir( "" );
        dir.mkdir( projectDirPath );
      }
      project->projectDir = projectDirPath;
      return projectDir.path();
    }
    else
    {
      return project->projectDir;
    }
  }
  return QStringLiteral();
}

QString MerginApi::getFullProjectName( QString projectNamespace, QString projectName )
{
  return QString( "%1/%2" ).arg( projectNamespace ).arg( projectName );
}

MerginApiStatus::VersionStatus MerginApi::apiVersionStatus() const
{
  return mApiVersionStatus;
}

void MerginApi::setApiVersionStatus( const MerginApiStatus::VersionStatus &apiVersionStatus )
{
  mApiVersionStatus = apiVersionStatus;
  emit apiVersionStatusChanged();
}

int MerginApi::userId() const
{
  return mUserId;
}

void MerginApi::setUserId( int userId )
{
  mUserId = userId;
}

int MerginApi::storageLimit() const
{
  return mStorageLimit;
}

int MerginApi::diskUsage() const
{
  return mDiskUsage;
}

void MerginApi::pingMergin()
{
  if ( mApiVersionStatus == MerginApiStatus::OK ) return;

  setApiVersionStatus( MerginApiStatus::PENDING );

  QNetworkRequest request;
  QUrl url( mApiRoot + QStringLiteral( "/ping" ) );
  request.setUrl( url );

  QNetworkReply *reply = mManager.get( request );
  InputUtils::log( url.toString(), QStringLiteral( "STARTED" ) );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::pingMerginReplyFinished );
}

QString MerginApi::apiRoot() const
{
  return mApiRoot;
}

void MerginApi::setApiRoot( const QString &apiRoot )
{
  QSettings settings;
  settings.beginGroup( QStringLiteral( "Input/" ) );
  if ( apiRoot.isEmpty() )
  {
    mApiRoot = defaultApiRoot();
  }
  else
  {
    mApiRoot = apiRoot;
  }
  settings.setValue( QStringLiteral( "apiRoot" ), mApiRoot );
  settings.endGroup();
  setApiVersionStatus( MerginApiStatus::UNKNOWN );
  emit apiRootChanged();
}

QString MerginApi::username() const
{
  return mUsername;
}

ProjectList MerginApi::projects()
{
  return mMerginProjects;
}

void MerginApi::listProjectsReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    if ( mMerginProjects.isEmpty() )
    {
      mMerginProjects = parseAllProjectsMetadata();
    }

    QByteArray data = r->readAll();
    ProjectList serverProjects = parseListProjectsMetadata( data );
    mMerginProjects = updateMerginProjectList( serverProjects );
    emit merginProjectsChanged();
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
  }
  else
  {
    QString message = QStringLiteral( "Network API error: %1(): %2" ).arg( QStringLiteral( "listProjects" ), r->errorString() );
    emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: listProjects" ) );
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( r->errorString() ) );

    if ( r->errorString() == QLatin1String( "Host requires authentication" ) )
    {
      emit authRequested();
      return;
    }
  }

  r->deleteLater();
  emit listProjectsFinished( mMerginProjects );
}

void MerginApi::continueDownloadFiles( const QString &projectFullName, const QString &version, int lastChunkNo, bool successfully )
{
  if ( !successfully )
  {
    QDir( mDataDir + TEMP_FOLDER + projectFullName ).removeRecursively();
    return;
  }

  MerginFile currentFile = mFilesToDownload[projectFullName].first();
  if ( lastChunkNo + 1 <= currentFile.chunks.size() - 1 )
  {
    downloadFile( projectFullName, currentFile.path, version, lastChunkNo + 1 );
  }
  else
  {
    mFilesToDownload[projectFullName].removeFirst();
    if ( !mFilesToDownload[projectFullName].isEmpty() )
    {
      MerginFile nextFile = mFilesToDownload[projectFullName].first();

      if ( !nextFile.size )
      {
        createEmptyFile( mDataDir + TEMP_FOLDER + projectFullName + "/" + nextFile.path );
        emit continueDownloadFiles( projectFullName, version, 0, true );
      }
      else
      {
        downloadFile( projectFullName, nextFile.path, version, 0 );
      }
    }
    else
    {
      mFilesToDownload.remove( projectFullName );
      QString projectNamespace;
      QString projectName;
      extractProjectName( projectFullName, projectNamespace, projectName );
      QString projectDir = getProjectDir( projectNamespace, projectName );
      copyTempFilesToProject( projectDir, projectFullName );

      emit syncProjectFinished( projectDir, projectFullName, true );
    }
  }
}

void MerginApi::downloadFileReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = mPendingRequests.value( r->url() );
  mPendingRequests.remove( r->url() );

  QUrlQuery query( r->url().query() );
  QString filename = query.queryItemValue( "file" );
  QString version = query.queryItemValue( "version" );
  int chunkNo = r->request().attribute( QNetworkRequest::User ).toInt();

  if ( r->error() == QNetworkReply::NoError )
  {
    QString projectNamespace;
    QString projectName;
    extractProjectName( projectFullName, projectNamespace, projectName );
    QString projectDir = getProjectDir( projectNamespace, projectName );

    bool overwrite = true; // chunkNo == 0
    bool closeFile = false;

    QList<MerginFile> files = mFilesToDownload.value( projectFullName );
    if ( !files.isEmpty() )
    {
      MerginFile file = mFilesToDownload.value( projectFullName ).first();
      overwrite  = file.chunks.size() <= 1;

      if ( chunkNo == file.chunks.size() - 1 )
      {
        closeFile = true;
      }
    }

    QString tempFoler = mDataDir + TEMP_FOLDER + projectFullName;
    createPathIfNotExists( tempFoler );
    handleOctetStream( r, tempFoler, filename, closeFile, overwrite );
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
    // Send another request afterwards
    emit downloadFileFinished( projectFullName, version, chunkNo, true );
  }
  else
  {
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( r->errorString() ) );
    emit downloadFileFinished( projectFullName, version, chunkNo, false );
    emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: downloadFile" ) );
  }

  r->deleteLater();
}

void MerginApi::uploadStartReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = mPendingRequests.value( r->url() );
  mPendingRequests.remove( r->url() );
  mOpenConnections.remove( projectFullName );

  if ( r->error() == QNetworkReply::NoError )
  {
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
    QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    QString transactionUUID;
    if ( doc.isObject() )
    {
      QJsonObject docObj = doc.object();
      transactionUUID = docObj.value( QStringLiteral( "transaction" ) ).toString();
      mTransactions.insert( projectFullName, transactionUUID );
    }

    QList<MerginFile> files = mFilesToUpload.value( projectFullName );
    if ( !files.isEmpty() )
    {
      MerginFile file = files.first();
      uploadFile( projectFullName, transactionUUID, file );
    }
    // Suppose to not happend if upload has been invoked
    else
    {
      uploadCancel( projectFullName );
    }
  }
  else
  {
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( r->errorString() ) );
    emit syncProjectFinished( QStringLiteral(), projectFullName, false );
    emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: uploadStartReply" ) );
  }

  r->deleteLater();
}

void MerginApi::uploadFileReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = mPendingRequests.value( r->url() );
  mPendingRequests.remove( r->url() );
  mOpenConnections.remove( projectFullName );

  QStringList params = ( r->url().toString().split( "/" ) );
  QString transactionUUID = params.at( params.length() - 2 );
  QString chunkID = params.at( params.length() - 1 );

  if ( r->error() == QNetworkReply::NoError )
  {
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
    MerginFile currentFile = mFilesToUpload[projectFullName].first();
    int chunkNo = currentFile.chunks.indexOf( chunkID );
    if ( chunkNo < currentFile.chunks.size() - 1 )
    {
      uploadFile( projectFullName, transactionUUID, currentFile, chunkNo + 1 );
    }
    else
    {
      mFilesToUpload[projectFullName].removeFirst();
      if ( !mFilesToUpload[projectFullName].isEmpty() )
      {
        MerginFile nextFile = mFilesToUpload[projectFullName].first();
        uploadFile( projectFullName, transactionUUID, nextFile );
      }
      else
      {
        mFilesToUpload.remove( projectFullName );
        uploadFinish( projectFullName, transactionUUID );
      }
    }
  }
  else
  {
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( r->errorString() ) );
    emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: downloadFile" ) );
    uploadCancel( projectFullName );
  }

  r->deleteLater();
}

void MerginApi::updateInfoReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QUrl url = r->url();
  mPendingRequests.remove( url );

  QString projectNamespace;
  QString projectName;
  extractProjectName( url.path(), projectNamespace, projectName );
  QString projectFullName = getFullProjectName( projectNamespace, projectName );
  QList<MerginFile> filesToDownload;

  QPair<QHash<QString, QList<MerginFile>>, QString> pair = parseAndCompareProjectFiles( r, true );
  QHash<QString, QList<MerginFile>> files = pair.first;
  QString version = pair.second;

  if ( r->error() != QNetworkReply::NoError )
  {
    emit syncProjectFinished( QStringLiteral(), projectFullName, false );
  }

  for ( QString key : files.keys() )
  {
    if ( key == QStringLiteral( "added" ) )
    {
      // no removal before upload
      if ( !mWaitingForUpload.contains( projectFullName ) )
      {
        QSet<QString> obsoleteFiles;
        for ( MerginFile file : files.value( key ) )
        {
          obsoleteFiles.insert( file.path );
        }
        if ( !obsoleteFiles.isEmpty() )
        {
          QString projectPath = getProjectDir( projectNamespace, projectName );
          mObsoleteFiles.insert( projectPath, obsoleteFiles );
        }
      }
    }
    else
    {
      for ( MerginFile file : files.value( key ) )
      {
        qreal rawNoOfChunks = qreal( file.size ) / UPLOAD_CHUNK_SIZE;
        int noOfChunks = qCeil( rawNoOfChunks );
        file.chunks = generateChunkIds( noOfChunks ); // doesnt really matter whats there, only how many chunks are expected
        filesToDownload << file;
      }
    }
  }
  if ( !filesToDownload.isEmpty() )
  {
    mFilesToDownload.insert( projectFullName, filesToDownload );
    MerginFile nextFile = filesToDownload.first();
    if ( !nextFile.size )
    {
      createEmptyFile( mDataDir + TEMP_FOLDER + projectFullName + "/" + nextFile.path );
      emit continueDownloadFiles( projectFullName, version, 0, true );
    }
    else
    {
      downloadFile( projectFullName, nextFile.path, version, 0 );
    }
  }
}

void MerginApi::uploadInfoReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QPair<QHash<QString, QList<MerginFile>>, QString> pair = parseAndCompareProjectFiles( r, false );
  QHash<QString, QList<MerginFile>> files = pair.first;
  QString version = pair.second;

  QUrl url = r->url();
  mPendingRequests.remove( url );
  QString projectNamespace;
  QString projectName;
  extractProjectName( url.path(), projectNamespace, projectName );
  QString projectFullName = getFullProjectName( projectNamespace, projectName );

  if ( r->error() != QNetworkReply::NoError )
  {
    emit syncProjectFinished( QStringLiteral(), projectFullName, false );
  }

  std::shared_ptr<MerginProject> project = getProject( projectFullName );
  QString projectPath = getProjectDir( projectNamespace, projectName ) + "/";

  QJsonDocument jsonDoc;
  QJsonObject changes;

  QList<MerginFile> filesToUpload;
  for ( QString key : files.keys() )
  {
    QJsonArray jsonArray;
    for ( MerginFile file : files.value( key ) )
    {
      QJsonObject fileObject;
      fileObject.insert( "path", file.path );
      fileObject.insert( "checksum", file.checksum );
      fileObject.insert( "size", file.size );
      fileObject.insert( "mtime", file.mtime.toString( Qt::ISODateWithMs ) );

      qreal rawNoOfChunks = qreal( file.size ) / UPLOAD_CHUNK_SIZE;
      int noOfChunks = qCeil( rawNoOfChunks );
      QStringList chunks = generateChunkIds( noOfChunks );
      QJsonArray chunksJson;
      for ( QString id : chunks )
      {
        chunksJson.append( id );
      }
      file.chunks = chunks;
      fileObject.insert( "chunks", chunksJson );
      jsonArray.append( fileObject );

      if ( key != QStringLiteral( "removed" ) )
      {
        filesToUpload.append( file );
      }
    }
    changes.insert( key, jsonArray );
  }

  r->deleteLater();
  mPendingRequests.remove( url );
  mFilesToUpload.insert( projectFullName, filesToUpload );

  QJsonObject json;
  json.insert( QStringLiteral( "changes" ), changes );
  json.insert( QStringLiteral( "version" ), version );
  jsonDoc.setObject( json );

  QString info = QString( "PUSH request - added: %1, updated: %2, removed: %3, renamed: %4" )
                 .arg( InputUtils::filesToString( files.value( "added" ) ) ).arg( InputUtils::filesToString( files.value( "updated" ) ) )
                 .arg( InputUtils::filesToString( files.value( "removed" ) ) ).arg( InputUtils::filesToString( files.value( "renamed" ) ) );
  InputUtils::log( url.toString(), info );

  uploadStart( projectFullName, jsonDoc.toJson( QJsonDocument::Compact ) );
}

void MerginApi::uploadFinishReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QUrl url = r->url();
  QString projectFullName = mPendingRequests.value( url );
  mPendingRequests.remove( url );
  mTransactions.remove( projectFullName );

  if ( r->error() == QNetworkReply::NoError )
  {
    QByteArray data = r->readAll();
    MerginProject project = readProjectMetadata( data );

    QString projectNamespace;
    QString projectName;
    extractProjectName( projectFullName, projectNamespace, projectName );
    QString projectDir = getProjectDir( projectNamespace, projectName ) + "/";
    mTempMerginProjects.insert( projectFullName, project );
    syncProjectFinished( projectDir, projectFullName, true );
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
  }
  else
  {
    QString message = QStringLiteral( "Network API error: %1(): %2" ).arg( QStringLiteral( "uploadFinish" ), r->errorString() );
    emit syncProjectFinished( QStringLiteral(), projectFullName, false );
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( message ) );
  }

  r->deleteLater();
}

void MerginApi::uploadCancelReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QUrl url = r->url();
  QString projectFullName = mPendingRequests.value( url );
  mPendingRequests.remove( url );

  if ( r->error() == QNetworkReply::NoError )
  {
    mTransactions.remove( projectFullName );
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
  }
  else
  {
    QString message = QStringLiteral( "Network API error: %1(): %2" ).arg( QStringLiteral( "uploadCancel" ), r->errorString() );
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( message ) );
  }

  emit syncProjectFinished( QStringLiteral(), projectFullName, false );
  r->deleteLater();
}

void MerginApi::getUserInfoFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
    QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      QJsonObject docObj = doc.object();
      mDiskUsage = docObj.value( QStringLiteral( "disk_usage" ) ).toInt();
      mStorageLimit = docObj.value( QStringLiteral( "storage_limit" ) ).toInt();
    }
  }
  else
  {
    QString message = QStringLiteral( "Network API error: %1(): %2" ).arg( QStringLiteral( "getUserInfo" ), r->errorString() );
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( message ) );
    emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: getUserInfo" ) );
  }

  r->deleteLater();
  emit userInfoChanged();
}

QPair<QHash<QString, QList<MerginFile>>, QString> MerginApi::parseAndCompareProjectFiles( QNetworkReply *r, bool isForUpdate )
{
  QHash<QString, QList<MerginFile>> files;
  QString version;
  if ( r->error() == QNetworkReply::NoError )
  {
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
    QList<MerginFile> added;
    QList<MerginFile> updatedFiles;
    QList<MerginFile> renamed;
    QList<MerginFile> removed;

    QUrl url = r->url();
    QString projectName;
    QString projectNamespace;
    extractProjectName( r->url().path(), projectNamespace, projectName );
    QString projectPath = getProjectDir( projectNamespace, projectName ) + "/";
    // List of files metadata of all files
    QList<MerginFile> projectFiles;

    QByteArray data = r->readAll();
    QJsonDocument doc = QJsonDocument::fromJson( data );
    if ( doc.isObject() )
    {
      QJsonObject docObj = doc.object();
      QString updated = docObj.value( QStringLiteral( "updated" ) ).toString();
      version = docObj.value( QStringLiteral( "version" ) ).toString();
      if ( version.isEmpty() )
      {
        version = QStringLiteral( "v1" );
      }
      auto it = docObj.constFind( QStringLiteral( "files" ) );
      QJsonValue v = *it;
      Q_ASSERT( v.isArray() );
      QJsonArray vArray = v.toArray();

      QSet<QString> localFiles = listFiles( projectPath );
      for ( auto it = vArray.constBegin(); it != vArray.constEnd(); ++it )
      {
        QJsonObject projectInfoMap = it->toObject();
        // Server metadata
        QString serverChecksum = projectInfoMap.value( QStringLiteral( "checksum" ) ).toString();
        QString path = projectInfoMap.value( QStringLiteral( "path" ) ).toString();
        QDateTime mtime = QDateTime::fromString( projectInfoMap.value( QStringLiteral( "mtime" ) ).toString(), Qt::ISODateWithMs ).toUTC();
        int size = projectInfoMap.value( QStringLiteral( "size" ) ).toInt();

        if ( isForUpdate )
        {
          // Include metadata of file from server in temp project's file
          MerginFile rawServerFile;
          rawServerFile.checksum = serverChecksum;
          rawServerFile.path = path;
          rawServerFile.size = size;
          rawServerFile.mtime = mtime;
          projectFiles << rawServerFile;
        }

        QByteArray localChecksumBytes = getChecksum( projectPath + path );
        QString localChecksum = QString::fromLatin1( localChecksumBytes.data(), localChecksumBytes.size() );
        QFileInfo info( projectPath + path );

        // removed
        if ( localChecksum.isEmpty() )
        {
          MerginFile file;
          file.checksum = serverChecksum;
          file.path = path;
          if ( isForUpdate )
          {
            file.size = size;
            file.mtime = mtime;
          }
          else
          {
            file.size = info.size(); // always 0
            file.mtime = info.lastModified(); // not relevant
          }
          removed.append( file );

        }
        // updated
        else if ( serverChecksum != localChecksum )
        {
          MerginFile file;
          // if updated file is required from server, it has to have server checksum
          // if updated file is going to be upload, it has to have local checksum
          if ( isForUpdate )
          {
            file.checksum = serverChecksum;
            file.mtime = mtime;
          }
          else
          {
            file.checksum = localChecksum;
            file.mtime = info.lastModified();
          }
          file.size = info.size();
          file.path = path;
          updatedFiles.append( file );

        }
        localFiles.remove( path );
      }

      // Rest of localFiles are newly added
      for ( QString p : localFiles )
      {
        MerginFile file;
        QByteArray localChecksumBytes = getChecksum( projectPath + p );
        QString localChecksum = QString::fromLatin1( localChecksumBytes.data(), localChecksumBytes.size() );
        file.checksum = localChecksum;
        file.path = p;
        QFileInfo info( projectPath + p );
        file.size = info.size();
        file.mtime = info.lastModified();
        added.append( file );
      }

      // Only if is for update, upload parses reply's data afterwards.
      if ( isForUpdate )
      {
        // Save data from server to update metadata after successful request
        MerginProject project;
        project.name = docObj.value( QStringLiteral( "name" ) ).toString();
        project.version = docObj.value( QStringLiteral( "version" ) ).toString();
        project.files = projectFiles;
        mTempMerginProjects.insert( projectNamespace + "/" + projectName, project );
      }
    }

    files.insert( QStringLiteral( "added" ), added );
    files.insert( QStringLiteral( "updated" ), updatedFiles );
    files.insert( QStringLiteral( "removed" ), removed );
    files.insert( QStringLiteral( "renamed" ), renamed );
  }
  else
  {
    QString message = QStringLiteral( "Network API error: %1(): %2" ).arg( QStringLiteral( "projectInfo" ), r->errorString() );
    emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: projectInfo" ) );
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( message ) );
  }

  return QPair<QHash<QString, QList<MerginFile>>, QString>( files, version );
}

ProjectList MerginApi::parseListProjectsMetadata( const QByteArray &data )
{
  ProjectList result;

  QJsonDocument doc = QJsonDocument::fromJson( data );
  if ( doc.isArray() )
  {
    QJsonArray vArray = doc.array();

    for ( auto it = vArray.constBegin(); it != vArray.constEnd(); ++it )
    {
      QJsonObject projectMap = it->toObject();
      MerginProject p;
      p.name = projectMap.value( QStringLiteral( "name" ) ).toString();
      p.projectNamespace = projectMap.value( QStringLiteral( "namespace" ) ).toString();
      p.creator = projectMap.value( QStringLiteral( "creator" ) ).toInt();

      QJsonValue meta = projectMap.value( QStringLiteral( "meta" ) );
      if ( meta.isObject() )
      {
        p.filesCount = meta.toObject().value( "files_count" ).toInt();
      }

      QJsonValue access = projectMap.value( QStringLiteral( "access" ) );
      if ( access.isObject() )
      {
        QJsonArray writers = access.toObject().value( "writers" ).toArray();
        for ( QJsonValueRef tag : writers )
        {
          p.writers.append( tag.toInt() );
        }
      }

      QJsonValue tags = projectMap.value( QStringLiteral( "tags" ) );
      if ( tags.isArray() )
      {
        for ( QJsonValueRef tag : tags.toArray() )
        {
          p.tags.append( tag.toString() );
        }
      }

      QDateTime updated = QDateTime::fromString( projectMap.value( QStringLiteral( "updated" ) ).toString(), Qt::ISODateWithMs ).toUTC();
      if ( !updated.isValid() )
      {
        p.serverUpdated = QDateTime::fromString( projectMap.value( QStringLiteral( "created" ) ).toString(), Qt::ISODateWithMs ).toUTC();
      }
      else
      {
        p.serverUpdated = updated;
      }

      result << std::make_shared<MerginProject>( p );
    }
  }
  return result;
}

std::shared_ptr<MerginProject> MerginApi::readProjectMetadataFromPath( const QString &projectPath )
{
  QFile file( QString( "%1/%2" ).arg( projectPath ).arg( sMetadataFile ) );
  if ( !file.exists() ) return std::shared_ptr<MerginProject>();

  QByteArray data;
  if ( file.open( QIODevice::ReadOnly ) )
  {
    data = file.readAll();
    file.close();
  }

  std::shared_ptr<MerginProject> p = std::make_shared<MerginProject>( readProjectMetadata( data ) );
  p->projectDir = projectPath;
  return p;
}

MerginProject MerginApi::readProjectMetadata( const QByteArray &data )
{
  QJsonDocument doc = QJsonDocument::fromJson( data );
  QJsonObject projectMap = doc.object();

  MerginProject p;
  p.name = projectMap.value( QStringLiteral( "name" ) ).toString();
  p.projectNamespace = projectMap.value( QStringLiteral( "namespace" ) ).toString();
  p.clientUpdated = QDateTime::fromString( projectMap.value( QStringLiteral( "clientUpdated" ) ).toString(), Qt::ISODateWithMs ).toUTC();
  p.lastSyncClient = QDateTime::fromString( projectMap.value( QStringLiteral( "lastSync" ) ).toString(), Qt::ISODateWithMs ).toUTC();
  p.version = projectMap.value( QStringLiteral( "version" ) ).toString();

  QList<MerginFile> projectFiles;
  auto it = projectMap.constFind( QStringLiteral( "files" ) );
  QJsonValue v = *it;
  Q_ASSERT( v.isArray() );
  QJsonArray vArray = v.toArray();
  for ( auto it = vArray.constBegin(); it != vArray.constEnd(); ++it )
  {
    QJsonObject projectInfoMap = it->toObject();
    QString serverChecksum = projectInfoMap.value( QStringLiteral( "checksum" ) ).toString();
    QString path = projectInfoMap.value( QStringLiteral( "path" ) ).toString();
    QDateTime mtime = QDateTime::fromString( projectInfoMap.value( QStringLiteral( "mtime" ) ).toString(), Qt::ISODateWithMs ).toUTC();
    int size = projectInfoMap.value( QStringLiteral( "size" ) ).toInt();

    // Include metadata of file from server in temp project's file
    MerginFile rawServerFile;
    rawServerFile.checksum = serverChecksum;
    rawServerFile.path = path;
    rawServerFile.size = size;
    rawServerFile.mtime = mtime;
    projectFiles << rawServerFile;
  }
  p.files = projectFiles;

  return p;
}

QJsonDocument MerginApi::createProjectMetadataJson( std::shared_ptr<MerginProject> project )
{
  QJsonDocument doc;
  QJsonObject projectMap;
  projectMap.insert( QStringLiteral( "clientUpdated" ), project->clientUpdated.toString( Qt::ISODateWithMs ) );
  projectMap.insert( QStringLiteral( "lastSync" ), project->lastSyncClient.toString( Qt::ISODateWithMs ) );
  projectMap.insert( QStringLiteral( "name" ), project->name );
  projectMap.insert( QStringLiteral( "namespace" ), project->projectNamespace );
  projectMap.insert( QStringLiteral( "version" ), project->version );

  QJsonArray filesArray;
  for ( MerginFile file : project->files )
  {
    QJsonObject fileObject;
    fileObject.insert( "path", file.path );
    fileObject.insert( "checksum", file.checksum );
    fileObject.insert( "size", file.size );
    fileObject.insert( "mtime", file.mtime.toString( Qt::ISODateWithMs ) );
    filesArray.append( fileObject );
  }
  projectMap.insert( QStringLiteral( "files" ), filesArray );

  doc.setObject( projectMap );
  return doc;
}

QStringList MerginApi::generateChunkIds( int noOfChunks )
{
  QStringList chunks;
  for ( int i = 0; i < noOfChunks; i++ )
  {
    QString chunkID = QUuid::createUuid().toString( QUuid::WithoutBraces );
    chunks.append( chunkID );
  }
  return chunks;
}

void MerginApi::updateProjectMetadata( const QString &projectDir, const QString &projectFullName, bool syncSuccessful )
{
  if ( !syncSuccessful )
  {
    mTempMerginProjects.remove( projectFullName );
    return;
  }

  MerginProject tempProjectData = mTempMerginProjects.take( projectFullName );
  std::shared_ptr<MerginProject> project = getProject( projectFullName );
  if ( project )
  {
    project->clientUpdated = project->serverUpdated;
    if ( project->projectDir.isEmpty() )
      project->projectDir = projectDir;
    project->lastSyncClient = QDateTime::currentDateTime().toUTC();
    project->files = tempProjectData.files;
    project->version = tempProjectData.version;

    QJsonDocument doc = createProjectMetadataJson( project );
    writeData( doc.toJson(), projectDir + "/" + MerginApi::sMetadataFile );

    emit merginProjectsChanged();
  }
}

void MerginApi::copyTempFilesToProject( const QString &projectDir, const QString &projectFullName )
{
  QString tempProjectDir = mDataDir + TEMP_FOLDER + projectFullName;
  InputUtils::cpDir( tempProjectDir, projectDir );
  QDir( tempProjectDir ).removeRecursively();
}

bool MerginApi::writeData( const QByteArray &data, const QString &path )
{
  QFile file( path );
  createPathIfNotExists( path );
  if ( !file.open( QIODevice::WriteOnly ) )
  {
    return false;
  }

  file.write( data );
  file.close();

  return true;
}

void MerginApi::continueWithUpload( const QString &projectDir, const QString &projectFullName, bool successfully )
{
  Q_UNUSED( projectDir )

  disconnect( this, &MerginApi::syncProjectFinished, this, &MerginApi::continueWithUpload );
  mWaitingForUpload.remove( projectFullName );

  if ( !successfully )
  {
    return;
  }

  // Has been canceled
  if ( !mTransactions.contains( projectFullName ) )
  {
    InputUtils::log( "continueWithUpload", QStringLiteral( "ABORT" ) );
    return;
  }

  QNetworkReply *reply = getProjectInfo( projectFullName );
  if ( reply )
    connect( reply, &QNetworkReply::finished, this, &MerginApi::uploadInfoReplyFinished );
}

void MerginApi::handleOctetStream( QNetworkReply *r, const QString &projectDir, const QString &filename, bool closeFile, bool overwrite )
{
  QByteArray data = r->readAll();
  QFile file;

  QString activeFilePath = projectDir + '/' + filename;
  file.setFileName( activeFilePath );
  createPathIfNotExists( activeFilePath );
  saveFile( data, file, closeFile, overwrite );
}

bool MerginApi::saveFile( const QByteArray &data, QFile &file, bool closeFile, bool overwrite )
{
  if ( !file.isOpen() )
  {
    if ( overwrite )
    {
      if ( !file.open( QIODevice::WriteOnly ) )
      {
        return false;
      }
    }
    else
    {
      if ( !file.open( QIODevice::Append ) )
      {
        return false;
      }
    }
  }

  file.write( data );
  if ( closeFile )
    file.close();

  return true;
}

void MerginApi::createPathIfNotExists( const QString &filePath )
{
  QDir dir;
  if ( !dir.exists( mDataDir ) )
    dir.mkpath( mDataDir );

  QFileInfo newFile( filePath );
  if ( !newFile.absoluteDir().exists() )
  {
    if ( !QDir( dir ).mkpath( newFile.absolutePath() ) )
    {
      InputUtils::log( QString( "Creating a folder failed for path: %1" ).arg( filePath ) );
    }
  }
}

void MerginApi::createEmptyFile( const QString &path )
{
  QDir dir;
  QFileInfo info( path );
  QString parentDir( info.dir().path() );
  if ( !dir.exists( parentDir ) )
    dir.mkpath( parentDir );

  QFile file( path );
  file.open( QIODevice::ReadWrite );
  file.close();
}

ProjectStatus MerginApi::getProjectStatus( std::shared_ptr<MerginProject> project, const QDateTime &lastModified )
{
  // There was no sync yet
  if ( !project->clientUpdated.isValid() )
  {
    return ProjectStatus::NoVersion;
  }

  // Something has locally changed after last sync with server
  int filesCount = getProjectFilesCount( project->projectDir );
  if ( project->lastSyncClient < lastModified || project->filesCount != filesCount )
  {
    return ProjectStatus::Modified;
  }

  // Version is lower than latest one, last sync also before updated
  if ( project->clientUpdated < project->serverUpdated && project->serverUpdated > project->lastSyncClient )
  {
    return ProjectStatus::OutOfDate;
  }

  return ProjectStatus::UpToDate;
}

QDateTime MerginApi::getLastModifiedFileDateTime( const QString &path )
{
  QDateTime lastModified;
  QDirIterator it( path, QStringList() << QStringLiteral( "*" ), QDir::Files, QDirIterator::Subdirectories );
  while ( it.hasNext() )
  {
    it.next();
    if ( !isInIgnore( it.fileInfo() ) )
    {
      if ( it.fileInfo().lastModified() > lastModified )
      {
        lastModified = it.fileInfo().lastModified();
      }
    }
  }
  return lastModified.toUTC();
}

int MerginApi::getProjectFilesCount( const QString &path )
{
  int count = 0;
  QDirIterator it( path, QStringList() << QStringLiteral( "*" ), QDir::Files, QDirIterator::Subdirectories );
  while ( it.hasNext() )
  {
    it.next();
    if ( !isInIgnore( it.fileInfo() ) )
    {
      count++;
    }
  }
  return count;
}

bool MerginApi::isInIgnore( const QFileInfo &info )
{
  return mIgnoreExtensions.contains( info.suffix() ) || mIgnoreFiles.contains( info.fileName() );
}

QByteArray MerginApi::getChecksum( const QString &filePath )
{
  QFile f( filePath );
  if ( f.open( QFile::ReadOnly ) )
  {
    QCryptographicHash hash( QCryptographicHash::Sha1 );
    QByteArray chunk = f.read( CHUNK_SIZE );
    while ( !chunk.isEmpty() )
    {
      hash.addData( chunk );
      chunk = f.read( CHUNK_SIZE );
    }
    f.close();
    return hash.result().toHex();
  }

  return QByteArray();
}

QSet<QString> MerginApi::listFiles( const QString &path )
{
  QSet<QString> files;
  QDirIterator it( path, QStringList() << QStringLiteral( "*" ), QDir::Files, QDirIterator::Subdirectories );
  while ( it.hasNext() )
  {
    it.next();
    if ( !isInIgnore( it.fileInfo() ) )
    {
      files << it.filePath().replace( path, "" );
    }
  }
  return files;
}
