#include "merginapi.h"

#include <QtNetwork>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>
#include <QByteArray>
#include <QSet>
#include <QMessageBox>

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
}

void MerginApi::clearTokenData()
{
  mTokenExpiration = QDateTime().currentDateTime().addDays( -42 ); // to make it expired arbitrary days ago
  mAuthToken.clear();
}

void MerginApi::downloadProjectFiles( const QString &projectFullName, const QByteArray &json )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QNetworkRequest request;
  QUrl url( mApiRoot + QStringLiteral( "/v1/project/fetch/" ) + projectFullName );
  request.setUrl( url );
  request.setRawHeader( "Authorization", QByteArray( "Bearer " + mAuthToken ) );
  request.setRawHeader( "Content-Type", "application/json" );
  request.setRawHeader( "Accept", "application/json" );
  mPendingRequests.insert( url, projectFullName );

  QNetworkReply *reply = mManager.post( request, json );
  InputUtils::log( url.toString(), QStringLiteral( "STARTED" ) );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::downloadProjectReplyFinished );
}

void MerginApi::uploadProjectFiles( const QString &projectNamespace, const QString &projectName, const QByteArray &json, const QList<MerginFile> &files )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    emit networkErrorOccurred( QStringLiteral( "Auth token is invalid" ), QStringLiteral( "Mergin API error: fetchProject" ) );
    return;
  }

  QString projectDir = getProjectDir( projectNamespace, projectName );
  QString projectPath = QString( projectDir + "/" );
  QHttpMultiPart *multiPart = new QHttpMultiPart( QHttpMultiPart::FormDataType );

  QHttpPart textPart;
  textPart.setHeader( QNetworkRequest::ContentDispositionHeader, QVariant( QStringLiteral( "form-data; name=\"changes\"" ) ) );
  textPart.setBody( json );
  multiPart->append( textPart );

  for ( MerginFile file : files )
  {
    QHttpPart filePart;
    filePart.setHeader( QNetworkRequest::ContentDispositionHeader, QStringLiteral( "form-data; name=\"%1\"; filename=\"%2\"" ).arg( file.path, file.path ) );
    filePart.setHeader( QNetworkRequest::ContentTypeHeader, QVariant( QStringLiteral( "multipart/form-data" ) ) );
    QFile *f = new QFile( projectPath + file.path );
    f->open( QIODevice::ReadOnly );
    filePart.setBodyDevice( f );
    f->setParent( multiPart ); // we cannot delete the file now, so delete it with the multiPart
    multiPart->append( filePart );
  }

  QString projectFullName = getFullProjectName( projectNamespace, projectName );
  QNetworkRequest request;
  QUrl url( mApiRoot + QString( "/v1/project/data_sync/%1" ).arg( projectFullName ) );
  request.setUrl( url );
  request.setRawHeader( "Authorization", QByteArray( "Bearer " + mAuthToken ) );
  mPendingRequests.insert( url, projectFullName );

  QNetworkReply *reply = mManager.post( request, multiPart );
  InputUtils::log( url.toString(), QStringLiteral( "STARTED" ) );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::uploadProjectReplyFinished );
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
      project->status = getProjectStatus( project->clientUpdated, project->serverUpdated, project->lastSyncClient, lastModified );
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
    emit notify( QStringLiteral( "Project created" ) );
    emit projectCreated( projectFullName );
  }
  else
  {
    qDebug() << r->errorString();
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
    emit notify( QStringLiteral( "Project deleted" ) );
    emit serverProjectDeleted( projectFullName );
  }
  else
  {
    qDebug() << r->errorString();
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
    qDebug() << r->errorString();
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
    QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      QJsonObject obj = doc.object();
      apiVersion = obj.value( QStringLiteral( "version" ) ).toString();
    }
  }
  else
  {
    msg = r->errorString();
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
    qDebug( "%s", message.toStdString().c_str() );
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

void MerginApi::downloadProjectReplyFinished()
{

  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = mPendingRequests.value( r->url() );
  QString projectNamespace;
  QString projectName;
  extractProjectName( projectFullName, projectNamespace, projectName );
  QString projectDir = getProjectDir( projectNamespace, projectName );
  if ( r->error() == QNetworkReply::NoError )
  {
    bool waitingForUpload = mWaitingForUpload.contains( projectFullName );

    deleteObsoleteFiles( projectDir + "/" );
    handleDataStream( r, projectDir, !waitingForUpload );
    emit syncProjectFinished( projectDir, projectFullName );
    if ( !waitingForUpload )
    {
      emit reloadProject( projectDir );
      emit notify( QStringLiteral( "Download successful" ) );
    }
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
  }
  else
  {
    qDebug() << r->errorString();
    emit syncProjectFinished( projectDir, projectFullName, false );
    emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: downloadProject" ) );
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( r->errorString() ) );
  }
  mPendingRequests.remove( r->url() );
  r->deleteLater();
}

void MerginApi::uploadProjectReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = mPendingRequests.value( r->url() );
  QString projectNamespace;
  QString projectName;
  extractProjectName( projectFullName, projectNamespace, projectName );
  QString projectDir = getProjectDir( projectNamespace, projectName );

  if ( r->error() == QNetworkReply::NoError )
  {
    MerginProject project = readProjectMetadata( r->readAll() );
    mTempMerginProjects.insert( projectFullName, project );

    emit syncProjectFinished( projectDir, projectFullName );
    emit reloadProject( projectDir );
    emit notify( QStringLiteral( "Upload successful" ) );
    InputUtils::log( r->url().toString(), QStringLiteral( "FINISHED" ) );
  }
  else
  {
    qDebug() << r->errorString();
    emit syncProjectFinished( projectDir, projectFullName, false );
    emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: uploadProject" ) );
    InputUtils::log( r->url().toString(), QStringLiteral( "FAILED - %1" ).arg( r->errorString() ) );
  }
  mPendingRequests.remove( r->url() );
  r->deleteLater();
}

void MerginApi::updateInfoReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QUrl url = r->url();
  InputUtils::log( url.toString(), QStringLiteral( "FINISHED" ) );
  QHash<QString, QList<MerginFile>> files = parseAndCompareProjectFiles( r, true );
  QString projectNamespace;
  QString projectName;
  extractProjectName( url.path(), projectNamespace, projectName );
  QString projectFullName = getFullProjectName( projectNamespace, projectName );

  QJsonDocument jsonDoc;
  QJsonArray fileArray;
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
        QJsonObject fileObject;
        fileObject.insert( "path", file.path );
        fileObject.insert( "checksum", file.checksum );
        fileObject.insert( "mtime", file.mtime.toString( Qt::ISODateWithMs ) );
        fileArray.append( fileObject );
      }
    }
  }

  r->deleteLater();
  mPendingRequests.remove( url );
  jsonDoc.setArray( fileArray );
  QString info = QString( "PUSH request - removed: %1, updated: %2, added: %3, renamed: %4" )
                 .arg( InputUtils::filesToString( files.value( "added" ) ) ).arg( InputUtils::filesToString( files.value( "updated" ) ) )
                 .arg( InputUtils::filesToString( files.value( "removed" ) ) ).arg( InputUtils::filesToString( files.value( "renamed" ) ) );

  InputUtils::log( url.toString(), info );

  downloadProjectFiles( projectFullName, jsonDoc.toJson( QJsonDocument::Compact ) );
}

void MerginApi::uploadInfoReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QUrl url = r->url();
  InputUtils::log( url.toString(), QStringLiteral( "FINISHED" ) );
  QHash<QString, QList<MerginFile>> files = parseAndCompareProjectFiles( r, false );
  QString projectNamespace;
  QString projectName;
  extractProjectName( url.path(), projectNamespace, projectName );
  QString projectFullName = getFullProjectName( projectNamespace, projectName );

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
  jsonDoc.setObject( changes );
  QString info = QString( "PULL request - added: %1, updated: %2, removed: %3, renamed: %4" )
                 .arg( InputUtils::filesToString( files.value( "added" ) ) ).arg( InputUtils::filesToString( files.value( "updated" ) ) )
                 .arg( InputUtils::filesToString( files.value( "removed" ) ) ).arg( InputUtils::filesToString( files.value( "renamed" ) ) );

  InputUtils::log( url.toString(), info );
  uploadProjectFiles( projectNamespace, projectName, jsonDoc.toJson( QJsonDocument::Compact ), filesToUpload );
}

void MerginApi::getUserInfoFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
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
    qDebug( "%s", message.toStdString().c_str() );
    emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: getUserInfo" ) );
  }

  r->deleteLater();
  emit userInfoChanged();
}

QHash<QString, QList<MerginFile>> MerginApi::parseAndCompareProjectFiles( QNetworkReply *r, bool isForUpdate )
{
  QHash<QString, QList<MerginFile>> files;
  if ( r->error() == QNetworkReply::NoError )
  {
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
          file.size = info.size();
          removed.append( file );
          localFiles.remove( path );
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
          localFiles.remove( path );
        }
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
    QString message = QStringLiteral( "Network API error: %1(): %2" ).arg( QStringLiteral( "listProjects" ), r->errorString() );
    qDebug( "%s", message.toStdString().c_str() );
    emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: projectInfo" ) );
  }

  return files;
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
  p.clientUpdated = QDateTime::fromString( projectMap.value( QStringLiteral( "clientUpdated" ) ).toString(), Qt::ISODateWithMs ).toUTC();;
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

void MerginApi::parseProjectInfoReply()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = mPendingRequests.value( r->url() );
  mPendingRequests.remove( r->url() );

  if ( r->error() == QNetworkReply::NoError )
  {
    QByteArray data = r->readAll();
    MerginProject project = readProjectMetadata( data );
    mTempMerginProjects.insert( projectFullName, project );
  }
  else
  {
    QString message = QStringLiteral( "Network API error: %1(): %2" ).arg( QStringLiteral( "listProjects" ), r->errorString() );
    qDebug( "%s", message.toStdString().c_str() );
    emit networkErrorOccurred( r->errorString(), QStringLiteral( "Mergin API error: projectInfo" ) );
  }
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

  QNetworkReply *reply = getProjectInfo( projectFullName );
  if ( reply )
    connect( reply, &QNetworkReply::finished, this, &MerginApi::uploadInfoReplyFinished );
}

void MerginApi::handleDataStream( QNetworkReply *r, const QString &projectDir, bool overwrite )
{
  // Read content type from reply's header
  QByteArray contentType;
  QString contentTypeString;
  QList<QByteArray> headerList = r->rawHeaderList();
  QString headString;
  foreach ( QByteArray head, headerList )
  {
    headString = QString::fromLatin1( head.data(), head.size() );
    if ( headString == QStringLiteral( "Content-Type" ) )
    {
      contentType = r->rawHeader( head );
      contentTypeString = QString::fromLatin1( contentType.data(), contentType.size() );
    }
  }

  // Read boundary hash from content types
  QString boundary;
  QRegularExpression re;
  re.setPattern( QStringLiteral( "[^;]+; boundary=(?<boundary>.+)" ) );
  QRegularExpressionMatch match = re.match( contentTypeString );
  if ( match.hasMatch() )
  {
    boundary = match.captured( "boundary" );
  }

  // Depends on boundary size itself + special chars which number may vary
  int boundarySize = boundary.length() + 8;
  QRegularExpression boundaryPattern( QStringLiteral( "(\r\n)?--" ) + boundary + QStringLiteral( "\r\n" ) );
  QRegularExpression headerPattern( QStringLiteral( "Content-Disposition: form-data; name=\"(?P<name>[^'\"]+)\"(; filename=\"(?P<filename>[^\"]+)\")?\r\n(Content-Type: (?P<content_type>.+))?\r\n" ) );
  QRegularExpression endPattern( QStringLiteral( "(\r\n)?--" ) + boundary +  QStringLiteral( "--\r\n" ) );

  QByteArray data;
  QString dataString;
  QString activeFilePath;
  QFile activeFile;

  while ( true )
  {
    QByteArray chunk = r->read( CHUNK_SIZE );
    if ( chunk.isEmpty() )
    {
      // End of stream - write rest of data to active file
      if ( !activeFile.fileName().isEmpty() )
      {
        QRegularExpressionMatch endMatch = endPattern.match( data );
        int tillIndex = data.indexOf( endMatch.captured( 0 ) );
        saveFile( data.left( tillIndex ), activeFile, true );
      }
      return;
    }

    data = data.append( chunk );
    dataString = QString::fromLatin1( data.data(), data.size() );
    QRegularExpressionMatch boundaryMatch = boundaryPattern.match( dataString );

    while ( boundaryMatch.hasMatch() )
    {
      if ( !activeFile.fileName().isEmpty() )
      {
        int tillIndex = data.indexOf( boundaryMatch.captured( 0 ) );
        saveFile( data.left( tillIndex ), activeFile, true );
      }

      // delete previously written data with next boundary part
      int tillIndex = data.indexOf( boundaryMatch.captured( 0 ) ) + boundaryMatch.captured( 0 ).length();
      data = data.remove( 0, tillIndex );
      dataString = QString::fromLatin1( data.data(), data.size() );

      QRegularExpressionMatch headerMatch = headerPattern.match( dataString );
      if ( !headerMatch.hasMatch() )
      {
        qDebug() << "Received corrupted header";
        data = data + r->read( CHUNK_SIZE );
        dataString = QString::fromLatin1( data.data(), data.size() );
      }
      headerMatch = headerPattern.match( dataString );
      data = data.remove( 0, headerMatch.captured( 0 ).length() );
      dataString = QString::fromLatin1( data.data(), data.size() );
      QString filename = headerMatch.captured( QStringLiteral( "filename" ) );

      activeFilePath = projectDir + '/' + filename;
      activeFile.setFileName( activeFilePath );
      if ( activeFile.exists() )
      {
        if ( overwrite )
        {
          // Remove file if want to override
          activeFile.remove();
        }
        else
        {
          int i = 0;
          QString newPath =  activeFilePath + QStringLiteral( "_conflict_copy" );
          while ( QFile::exists( newPath + QString::number( i ) ) )
            ++i;
          if ( !QFile::copy( activeFilePath, newPath + QString::number( i ) ) )
          {
            qDebug() << activeFilePath << " will be overwritten";
          }
          activeFile.remove();
        }
      }
      else
      {
        createPathIfNotExists( activeFilePath );
      }

      boundaryMatch = boundaryPattern.match( dataString );
    }

    // Write rest of chunk to file
    if ( !activeFile.fileName().isEmpty() )
    {
      saveFile( data.left( data.size() - boundarySize ), activeFile, false );
    }
    data = data.remove( 0, data.size() - boundarySize );
  }
}

bool MerginApi::saveFile( const QByteArray &data, QFile &file, bool closeFile )
{
  if ( !file.isOpen() )
  {
    if ( !file.open( QIODevice::Append ) )
    {
      return false;
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
      qDebug() << "Creating folder failed";
  }
}

ProjectStatus MerginApi::getProjectStatus( const QDateTime &localUpdated, const QDateTime &updated, const QDateTime &lastSync, const QDateTime &lastModified )
{
  // There was no sync yet
  if ( !localUpdated.isValid() )
  {
    return ProjectStatus::NoVersion;
  }

  // Something has locally changed after last sync with server
  if ( lastSync < lastModified )
  {
    return ProjectStatus::Modified;
  }

  // Version is lower than latest one, last sync also before updated
  if ( localUpdated < updated && updated > lastSync.toUTC() )
  {
    return ProjectStatus::OutOfDate;
  }

  return ProjectStatus::UpToDate;
}

QDateTime MerginApi::getLastModifiedFileDateTime( const QString &path )
{
  QDateTime lastModified = QFileInfo( path ).lastModified();
  QDirIterator it( path, QStringList() << QStringLiteral( "*" ), QDir::Files, QDirIterator::Subdirectories );
  while ( it.hasNext() )
  {
    it.next();
    if ( !mIgnoreFiles.contains( it.fileInfo().suffix() ) )
    {
      if ( it.fileInfo().lastModified() > lastModified )
      {
        lastModified = it.fileInfo().lastModified();
      }
    }
  }
  return lastModified.toUTC();
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
    if ( !mIgnoreFiles.contains( it.fileInfo().suffix() ) )
    {
      files << it.filePath().replace( path, "" );
    }
  }
  return files;
}
