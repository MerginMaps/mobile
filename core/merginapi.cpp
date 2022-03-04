/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "merginapi.h"

#include <QtNetwork>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>
#include <QByteArray>
#include <QSet>
#include <QUuid>
#include <QtMath>

#include "coreutils.h"
#include "geodiffutils.h"
#include "localprojectsmanager.h"
#include "merginuserauth.h"
#include "merginuserinfo.h"
#include "merginsubscriptioninfo.h"

#include <geodiff.h>

const QString MerginApi::sMetadataFile = QStringLiteral( "/.mergin/mergin.json" );
const QString MerginApi::sMerginConfigFile = QStringLiteral( "mergin-config.json" );
const QString MerginApi::sDefaultApiRoot = QStringLiteral( "https://public.cloudmergin.com/" );
const QSet<QString> MerginApi::sIgnoreExtensions = QSet<QString>() << "gpkg-shm" << "gpkg-wal" << "qgs~" << "qgz~" << "pyc" << "swap";
const QSet<QString> MerginApi::sIgnoreImageExtensions = QSet<QString>() << "jpg" << "jpeg" << "png";
const QSet<QString> MerginApi::sIgnoreFiles = QSet<QString>() << "mergin.json" << ".DS_Store";
const int MerginApi::UPLOAD_CHUNK_SIZE = 10 * 1024 * 1024; // Should be the same as on Mergin server


MerginApi::MerginApi( LocalProjectsManager &localProjects, QObject *parent )
  : QObject( parent )
  , mLocalProjects( localProjects )
  , mDataDir( localProjects.dataDir() )
  , mUserInfo( new MerginUserInfo )
  , mSubscriptionInfo( new MerginSubscriptionInfo )
  , mUserAuth( new MerginUserAuth )
{
  qRegisterMetaType<Transactions>();

  QObject::connect( this, &MerginApi::authChanged, this, &MerginApi::saveAuthData );
  QObject::connect( this, &MerginApi::apiRootChanged, this, &MerginApi::pingMergin );
  QObject::connect( this, &MerginApi::pingMerginFinished, this, &MerginApi::checkMerginVersion );
  QObject::connect( mUserInfo, &MerginUserInfo::userInfoChanged, this, &MerginApi::userInfoChanged );
  QObject::connect( mSubscriptionInfo, &MerginSubscriptionInfo::subscriptionInfoChanged, this, &MerginApi::subscriptionInfoChanged );
  QObject::connect( mSubscriptionInfo, &MerginSubscriptionInfo::planProductIdChanged, this, &MerginApi::onPlanProductIdChanged );
  QObject::connect( mUserAuth, &MerginUserAuth::authChanged, this, &MerginApi::authChanged );

  loadAuthData();
  GEODIFF_init();
  GEODIFF_setLoggerCallback( &GeodiffUtils::log );
  GEODIFF_setMaximumLoggerLevel( GEODIFF_LoggerLevel::LevelDebug );
}

MerginUserAuth *MerginApi::userAuth() const
{
  return mUserAuth;
}

MerginUserInfo *MerginApi::userInfo() const
{
  return mUserInfo;
}

MerginSubscriptionInfo *MerginApi::subscriptionInfo() const
{
  return mSubscriptionInfo;
}

QString MerginApi::listProjects( const QString &searchExpression, const QString &flag, const QString &filterTag, const int page )
{
  bool authorize = !flag.isEmpty();
  if ( ( authorize && !validateAuthAndContinute() ) || mApiVersionStatus != MerginApiStatus::OK )
  {
    emit listProjectsFailed();
    return QString();
  }

  QUrlQuery query;
  if ( !filterTag.isEmpty() )
  {
    query.addQueryItem( "tags", filterTag );
  }
  if ( !searchExpression.isEmpty() )
  {
    query.addQueryItem( "name", searchExpression.toUtf8().toPercentEncoding() );
  }
  if ( !flag.isEmpty() )
  {
    query.addQueryItem( "flag", flag );
  }
  query.addQueryItem( "order_by", QStringLiteral( "namespace" ) );
  // Required query parameters
  query.addQueryItem( "page", QString::number( page ) );
  query.addQueryItem( "per_page", QString::number( PROJECT_PER_PAGE ) );

  QUrl url( mApiRoot + QStringLiteral( "/v1/project/paginated" ) );
  url.setQuery( query );

  // Even if the authorization is not required, it can be include to fetch more results
  QNetworkRequest request = getDefaultRequest( mUserAuth->hasAuthData() );
  request.setUrl( url );

  QString requestId = CoreUtils::uuidWithoutBraces( QUuid::createUuid() );

  QNetworkReply *reply = mManager.get( request );
  CoreUtils::log( "list projects", QStringLiteral( "Requesting: " ) + url.toString() );
  connect( reply, &QNetworkReply::finished, this, [this, requestId]() {this->listProjectsReplyFinished( requestId );} );

  return requestId;
}

QString MerginApi::listProjectsByName( const QStringList &projectNames )
{
  if ( mApiVersionStatus != MerginApiStatus::OK )
  {
    emit listProjectsFailed();
    return QLatin1String();
  }

  // Authentification is optional in this case, as there might be public projects without the need to be logged in
  validateAuthAndContinute();

  // construct JSON body
  QJsonDocument body;
  QJsonObject projects;
  QJsonArray projectsArr = QJsonArray::fromStringList( projectNames );

  projects.insert( "projects", projectsArr );
  body.setObject( projects );

  QUrl url( mApiRoot + QStringLiteral( "/v1/project/by_names" ) );

  QNetworkRequest request = getDefaultRequest( true );
  request.setUrl( url );
  request.setRawHeader( "Content-type", "application/json" );

  QString requestId = CoreUtils::uuidWithoutBraces( QUuid::createUuid() );

  QNetworkReply *reply = mManager.post( request, body.toJson() );
  CoreUtils::log( "list projects by name", QStringLiteral( "Requesting: " ) + url.toString() );
  connect( reply, &QNetworkReply::finished, this, [this, requestId]() {this->listProjectsByNameReplyFinished( requestId );} );

  return requestId;
}


void MerginApi::downloadNextItem( const QString &projectFullName )
{
  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];

  if ( transaction.downloadQueue.isEmpty() )
  {
    // there's nothing to download so just finalize the update
    finalizeProjectUpdate( projectFullName );
    return;
  }

  DownloadQueueItem item = transaction.downloadQueue.takeFirst();

  QUrl url( mApiRoot + QStringLiteral( "/v1/project/raw/" ) + projectFullName );
  QUrlQuery query;
  // Handles special chars in a filePath (e.g prevents to convert "+" sign into a space)
  query.addQueryItem( "file", item.filePath.toUtf8().toPercentEncoding() );
  query.addQueryItem( "version", QStringLiteral( "v%1" ).arg( item.version ) );
  if ( item.downloadDiff )
    query.addQueryItem( "diff", "true" );
  url.setQuery( query );

  QNetworkRequest request = getDefaultRequest();
  request.setUrl( url );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ), projectFullName );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrTempFileName ), item.tempFileName );

  QString range;
  if ( item.rangeFrom != -1 && item.rangeTo != -1 )
  {
    range = QStringLiteral( "bytes=%1-%2" ).arg( item.rangeFrom ).arg( item.rangeTo );
    request.setRawHeader( "Range", range.toUtf8() );
  }

  Q_ASSERT( !transaction.replyDownloadItem );
  transaction.replyDownloadItem = mManager.get( request );
  connect( transaction.replyDownloadItem, &QNetworkReply::finished, this, &MerginApi::downloadItemReplyFinished );

  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Requesting item: " ) + url.toString() +
                  ( !range.isEmpty() ? " Range: " + range : QString() ) );
}

void MerginApi::removeProjectsTempFolder( const QString &projectNamespace, const QString &projectName )
{
  if ( projectNamespace.isEmpty() || projectName.isEmpty() )
    return; // otherwise we could remove enitre users temp or entire .temp

  QString path = getTempProjectDir( getFullProjectName( projectNamespace, projectName ) );
  QDir( path ).removeRecursively();
}

QNetworkRequest MerginApi::getDefaultRequest( bool withAuth )
{
  QNetworkRequest request;
  QString info = CoreUtils::appInfo();
  request.setRawHeader( "User-Agent", QByteArray( info.toUtf8() ) );
  if ( withAuth )
    request.setRawHeader( "Authorization", QByteArray( "Bearer " + mUserAuth->authToken() ) );

  return request;
}

bool MerginApi::projectFileHasBeenUpdated( const ProjectDiff &diff )
{
  for ( QString filePath : diff.remoteAdded )
  {
    if ( hasProjecFileExtension( filePath ) )
      return true;
  }

  for ( QString filePath : diff.remoteUpdated )
  {
    if ( hasProjecFileExtension( filePath ) )
      return true;
  }

  return false;
}

bool MerginApi::hasProjecFileExtension( const QString filePath )
{
  return filePath.contains( ".qgs" ) || filePath.contains( ".qgz" );
}

bool MerginApi::supportsSelectiveSync() const
{
  return mSupportsSelectiveSync;
}

void MerginApi::setSupportsSelectiveSync( bool supportsSelectiveSync )
{
  mSupportsSelectiveSync = supportsSelectiveSync;
}

bool MerginApi::apiSupportsSubscriptions() const
{
  return mApiSupportsSubscriptions;
}

void MerginApi::setApiSupportsSubscriptions( bool apiSupportsSubscriptions )
{
  if ( mApiSupportsSubscriptions != apiSupportsSubscriptions )
  {
    mApiSupportsSubscriptions = apiSupportsSubscriptions;
    emit apiSupportsSubscriptionsChanged();
  }
}

#if !defined(USE_MERGIN_DUMMY_API_KEY)
#include "merginsecrets.cpp"
#endif

QString MerginApi::getApiKey( const QString &serverName )
{
#if defined(USE_MERGIN_DUMMY_API_KEY)
  Q_UNUSED( serverName );
#else
  QString secretKey = __getSecretApiKey( serverName );
  if ( !secretKey.isEmpty() )
    return secretKey;
#endif
  return "not-secret-key";
}

void MerginApi::downloadItemReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();
  QString tempFileName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrTempFileName ) ).toString();

  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];
  Q_ASSERT( r == transaction.replyDownloadItem );

  if ( r->error() == QNetworkReply::NoError )
  {
    QByteArray data = r->readAll();

    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Downloaded item (%1 bytes)" ).arg( data.size() ) );

    QString tempFolder = getTempProjectDir( projectFullName );
    QString tempFilePath = tempFolder + "/" + tempFileName;
    createPathIfNotExists( tempFilePath );

    // save to a tmp file, assemble at the end
    QFile file( tempFilePath );
    if ( file.open( QIODevice::WriteOnly ) )
    {
      file.write( data );
      file.close();
    }
    else
    {
      CoreUtils::log( "pull " + projectFullName, "Failed to open for writing: " + file.fileName() );
    }

    transaction.transferedSize += data.size();
    emit syncProjectStatusChanged( projectFullName, transaction.transferedSize / transaction.totalSize );

    transaction.replyDownloadItem->deleteLater();
    transaction.replyDownloadItem = nullptr;

    // Send another request (or finish)
    downloadNextItem( projectFullName );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    if ( serverMsg.isEmpty() )
    {
      serverMsg = r->errorString();
    }
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );

    transaction.replyDownloadItem->deleteLater();
    transaction.replyDownloadItem = nullptr;

    // get rid of the temporary download dir where we may have left some downloaded files
    QDir( getTempProjectDir( projectFullName ) ).removeRecursively();

    if ( transaction.firstTimeDownload )
    {
      Q_ASSERT( !transaction.projectDir.isEmpty() );
      QDir( transaction.projectDir ).removeRecursively();
    }

    finishProjectSync( projectFullName, false );

    emit networkErrorOccurred( QStringLiteral(), QStringLiteral( "Mergin API error: downloadFile" ) );
  }
}

void MerginApi::cacheServerConfig()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();

  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];
  Q_ASSERT( r == transaction.replyDownloadItem );

  if ( r->error() == QNetworkReply::NoError )
  {
    QByteArray data = r->readAll();

    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Downloaded mergin config (%1 bytes)" ).arg( data.size() ) );
    transaction.config = MerginConfig::fromJson( data );

    transaction.replyDownloadItem->deleteLater();
    transaction.replyDownloadItem = nullptr;

    prepareDownloadConfig( projectFullName, true );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    if ( serverMsg.isEmpty() )
    {
      serverMsg = r->errorString();
    }
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Failed to cache mergin config - %1. %2" ).arg( r->errorString(), serverMsg ) );

    transaction.replyDownloadItem->deleteLater();
    transaction.replyDownloadItem = nullptr;

    // get rid of the temporary download dir where we may have left some downloaded files
    CoreUtils::removeDir( getTempProjectDir( projectFullName ) );

    if ( transaction.firstTimeDownload )
    {
      CoreUtils::removeDir( transaction.projectDir );
    }

    finishProjectSync( projectFullName, false );

    emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: downloadFile" ) );
  }
}


void MerginApi::uploadFile( const QString &projectFullName, const QString &transactionUUID, MerginFile file, int chunkNo )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];

  QString chunkID = file.chunks.at( chunkNo );

  QString filePath;
  if ( file.diffName.isEmpty() )
    filePath = transaction.projectDir + "/" + file.path;
  else  // use diff file instead of full file
    filePath = transaction.projectDir + "/.mergin/" + file.diffName;

  QFile f( filePath );
  QByteArray data;

  if ( f.open( QIODevice::ReadOnly ) )
  {
    f.seek( chunkNo * UPLOAD_CHUNK_SIZE );
    data = f.read( UPLOAD_CHUNK_SIZE );
  }

  QNetworkRequest request = getDefaultRequest();
  QUrl url( mApiRoot + QStringLiteral( "/v1/project/push/chunk/%1/%2" ).arg( transactionUUID ).arg( chunkID ) );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/octet-stream" );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ), projectFullName );

  Q_ASSERT( !transaction.replyUploadFile );
  transaction.replyUploadFile = mManager.post( request, data );
  connect( transaction.replyUploadFile, &QNetworkReply::finished, this, &MerginApi::uploadFileReplyFinished );

  CoreUtils::log( "push " + projectFullName, QStringLiteral( "Uploading item: " ) + url.toString() );
}

void MerginApi::uploadStart( const QString &projectFullName, const QByteArray &json )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];

  QNetworkRequest request = getDefaultRequest();
  QUrl url( mApiRoot + QStringLiteral( "v1/project/push/%1" ).arg( projectFullName ) );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ), projectFullName );

  Q_ASSERT( !transaction.replyUploadStart );
  transaction.replyUploadStart = mManager.post( request, json );
  connect( transaction.replyUploadStart, &QNetworkReply::finished, this, &MerginApi::uploadStartReplyFinished );

  CoreUtils::log( "push " + projectFullName, QStringLiteral( "Starting push request: " ) + url.toString() );
}

void MerginApi::uploadCancel( const QString &projectFullName )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  if ( !mTransactionalStatus.contains( projectFullName ) )
    return;

  CoreUtils::log( "push " + projectFullName, QStringLiteral( "User requested cancel" ) );

  TransactionStatus &transaction = mTransactionalStatus[projectFullName];

  // There is an open transaction, abort it followed by calling cancelUpload again.
  if ( transaction.replyUploadProjectInfo )
  {
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Aborting project info request" ) );
    transaction.replyUploadProjectInfo->abort();  // will trigger uploadInfoReplyFinished slot and emit sync finished
  }
  else if ( transaction.replyUploadStart )
  {
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Aborting upload start" ) );
    transaction.replyUploadStart->abort();  // will trigger uploadStartReplyFinished slot and emit sync finished
  }
  else if ( transaction.replyUploadFile )
  {
    QString transactionUUID = transaction.transactionUUID;  // copy transaction uuid as the transaction object will be gone after abort
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Aborting upload file" ) );
    transaction.replyUploadFile->abort();  // will trigger uploadFileReplyFinished slot and emit sync finished

    // also need to cancel the transaction
    sendUploadCancelRequest( projectFullName, transactionUUID );
  }
  else if ( transaction.replyUploadFinish )
  {
    QString transactionUUID = transaction.transactionUUID;  // copy transaction uuid as the transaction object will be gone after abort
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Aborting upload finish" ) );
    transaction.replyUploadFinish->abort();  // will trigger uploadFinishReplyFinished slot and emit sync finished

    sendUploadCancelRequest( projectFullName, transactionUUID );
  }
  else
  {
    Q_ASSERT( false );  // unexpected state
  }
}


void MerginApi::sendUploadCancelRequest( const QString &projectFullName, const QString &transactionUUID )
{
  QNetworkRequest request = getDefaultRequest();
  QUrl url( mApiRoot + QStringLiteral( "v1/project/push/cancel/%1" ).arg( transactionUUID ) );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ), projectFullName );

  QNetworkReply *reply = mManager.post( request, QByteArray() );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::uploadCancelReplyFinished );
  CoreUtils::log( "push " + projectFullName, QStringLiteral( "Requesting upload transaction cancel: " ) + url.toString() );
}

void MerginApi::updateCancel( const QString &projectFullName )
{
  if ( !mTransactionalStatus.contains( projectFullName ) )
    return;

  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "User requested cancel" ) );

  TransactionStatus &transaction = mTransactionalStatus[projectFullName];

  if ( transaction.replyProjectInfo )
  {
    // we're still fetching project info
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Aborting project info request" ) );
    transaction.replyProjectInfo->abort();  // abort will trigger updateInfoReplyFinished() slot
  }
  else if ( transaction.replyDownloadItem )
  {
    // we're already downloading some files
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Aborting pending download" ) );
    transaction.replyDownloadItem->abort();  // abort will trigger downloadItemReplyFinished slot
  }
  else
  {
    Q_ASSERT( false );  // unexpected state
  }
}

void MerginApi::uploadFinish( const QString &projectFullName, const QString &transactionUUID )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];

  QNetworkRequest request = getDefaultRequest();
  QUrl url( mApiRoot + QStringLiteral( "v1/project/push/finish/%1" ).arg( transactionUUID ) );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ), projectFullName );

  Q_ASSERT( !transaction.replyUploadFinish );
  transaction.replyUploadFinish = mManager.post( request, QByteArray() );
  connect( transaction.replyUploadFinish, &QNetworkReply::finished, this, &MerginApi::uploadFinishReplyFinished );

  CoreUtils::log( "push " + projectFullName, QStringLiteral( "Requesting transaction finish: " ) + transactionUUID );
}

void MerginApi::updateProject( const QString &projectNamespace, const QString &projectName, bool withoutAuth )
{
  QString projectFullName = getFullProjectName( projectNamespace, projectName );

  CoreUtils::log( "pull " + projectFullName, "### Starting ###" );

  QNetworkReply *reply = getProjectInfo( projectFullName, withoutAuth );
  if ( reply )
  {
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Requesting project info: " ) + reply->request().url().toString() );

    Q_ASSERT( !mTransactionalStatus.contains( projectFullName ) );
    mTransactionalStatus.insert( projectFullName, TransactionStatus() );
    mTransactionalStatus[projectFullName].replyProjectInfo = reply;
    mTransactionalStatus[projectFullName].configAllowed = mSupportsSelectiveSync;

    emit syncProjectStatusChanged( projectFullName, 0 );

    connect( reply, &QNetworkReply::finished, this, &MerginApi::updateInfoReplyFinished );
  }
  else
  {
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "FAILED to create project info request!" ) );
  }
}

void MerginApi::uploadProject( const QString &projectNamespace, const QString &projectName, bool isInitialUpload )
{
  QString projectFullName = getFullProjectName( projectNamespace, projectName );

  CoreUtils::log( "push " + projectFullName, "### Starting ###" );

  QNetworkReply *reply = getProjectInfo( projectFullName );
  if ( reply )
  {
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Requesting project info: " ) + reply->request().url().toString() );

    // create entry about pending upload for the project
    Q_ASSERT( !mTransactionalStatus.contains( projectFullName ) );
    mTransactionalStatus.insert( projectFullName, TransactionStatus() );
    mTransactionalStatus[projectFullName].replyUploadProjectInfo = reply;
    mTransactionalStatus[projectFullName].isInitialUpload = isInitialUpload;
    mTransactionalStatus[projectFullName].configAllowed = mSupportsSelectiveSync;

    emit syncProjectStatusChanged( projectFullName, 0 );

    connect( reply, &QNetworkReply::finished, this, &MerginApi::uploadInfoReplyFinished );
  }
  else
  {
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "FAILED to create project info request!" ) );
  }
}

void MerginApi::authorize( const QString &login, const QString &password )
{
  if ( login.isEmpty() || password.isEmpty() )
  {
    emit authFailed();
    emit notify( QStringLiteral( "Please enter your login details" ) );
    return;
  }

  mUserAuth->blockSignals( true );
  mUserAuth->setPassword( password );
  mUserAuth->blockSignals( false );

  QNetworkRequest request = getDefaultRequest( false );
  QString urlString = mApiRoot + QStringLiteral( "v1/auth/login" );
  QUrl url( urlString );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );

  QJsonDocument jsonDoc;
  QJsonObject jsonObject;
  jsonObject.insert( QStringLiteral( "login" ), login );
  jsonObject.insert( QStringLiteral( "password" ), mUserAuth->password() );
  jsonDoc.setObject( jsonObject );
  QByteArray json = jsonDoc.toJson( QJsonDocument::Compact );

  QNetworkReply *reply = mManager.post( request, json );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::authorizeFinished );
  CoreUtils::log( "auth", QStringLiteral( "Requesting authorization: " ) + url.toString() );
}

void MerginApi::registerUser( const QString &username,
                              const QString &email,
                              const QString &password,
                              const QString &confirmPassword,
                              bool acceptedTOC )
{
  // Some very basic checks, so we do not validate everything
  if ( username.isEmpty() || username.length() < 4 )
  {
    emit registrationFailed();
    emit notify( tr( "Username must have at least 4 characters" ) );
    return;
  }

  if ( email.isEmpty() || !email.contains( '@' ) || !email.contains( '.' ) )
  {
    emit registrationFailed();
    emit notify( tr( "Please enter a valid email" ) );
    return;
  }

  if ( password.isEmpty() || password.length() < 8 )
  {
    emit registrationFailed();
    QString msg = tr( "Password not strong enough. It must"
                      "%1 be at least 8 characters long"
                      "%1 contain lowercase characters"
                      "%1 contain uppercase characters"
                      "%1 contain digits or special characters" )
                  .arg( "<br />  -" );
    emit notify( msg );
    return;
  }

  if ( confirmPassword != password )
  {
    emit registrationFailed();
    emit notify( tr( "Passwords do not match" ) );
    return;
  }

  if ( !acceptedTOC )
  {
    emit registrationFailed();
    emit notify( tr( "Please accept Terms and Privacy Policy" ) );
    return;
  }

  // request
  QNetworkRequest request = getDefaultRequest( false );
  QString urlString = mApiRoot + QStringLiteral( "v1/auth/register" );
  QUrl url( urlString );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );

  QJsonDocument jsonDoc;
  QJsonObject jsonObject;
  jsonObject.insert( QStringLiteral( "username" ), username );
  jsonObject.insert( QStringLiteral( "email" ), email );
  jsonObject.insert( QStringLiteral( "password" ), password );
  jsonObject.insert( QStringLiteral( "api_key" ), getApiKey( mApiRoot ) );
  jsonDoc.setObject( jsonObject );
  QByteArray json = jsonDoc.toJson( QJsonDocument::Compact );
  QNetworkReply *reply = mManager.post( request, json );
  connect( reply, &QNetworkReply::finished, this, [ = ]() { this->registrationFinished( username, password ); } );
  CoreUtils::log( "auth", QStringLiteral( "Requesting registration: " ) + url.toString() );
}

void MerginApi::getUserInfo( )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QNetworkRequest request = getDefaultRequest();
  QString urlString = mApiRoot + QStringLiteral( "v1/user/%1" ).arg( mUserAuth->username() );
  QUrl url( urlString );
  request.setUrl( url );

  QNetworkReply *reply = mManager.get( request );
  CoreUtils::log( "user info", QStringLiteral( "Requesting user info: " ) + url.toString() );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::getUserInfoFinished );
}

void MerginApi::getSubscriptionInfo()
{
  if ( !apiSupportsSubscriptions() )
  {
    qDebug() << "Subscription info request skipped - server doesn't support subscriptions.";
  }

  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QNetworkRequest request = getDefaultRequest();
  QString urlString = mApiRoot + QStringLiteral( "v1/user/service" );
  QUrl url( urlString );
  request.setUrl( url );

  QNetworkReply *reply = mManager.get( request );
  CoreUtils::log( "subscription info", QStringLiteral( "Requesting subscription info: " ) + url.toString() );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::getSubscriptionInfoFinished );
}

void MerginApi::clearAuth()
{
  mUserAuth->clear();
  mUserInfo->clear();
  mSubscriptionInfo->clear();
}

void MerginApi::resetApiRoot()
{
  QSettings settings;
  settings.beginGroup( QStringLiteral( "Input/" ) );
  setApiRoot( defaultApiRoot() );
  settings.endGroup();
}

QString MerginApi::resetPasswordUrl()
{
  if ( !mApiRoot.isEmpty() )
  {
    QUrl base( mApiRoot );
    return base.resolved( QUrl( "login/reset" ) ).toString();
  }
  return QString();
}

void MerginApi::createProject( const QString &projectNamespace, const QString &projectName, bool isPublic )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QString projectFullName = getFullProjectName( projectNamespace, projectName );

  QNetworkRequest request = getDefaultRequest();
  QUrl url( mApiRoot + QString( "/v1/project/%1" ).arg( projectNamespace ) );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );
  request.setRawHeader( "Accept", "application/json" );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ), projectFullName );

  QJsonDocument jsonDoc;
  QJsonObject jsonObject;
  jsonObject.insert( QStringLiteral( "name" ), projectName );
  jsonObject.insert( QStringLiteral( "public" ), isPublic );
  jsonDoc.setObject( jsonObject );
  QByteArray json = jsonDoc.toJson( QJsonDocument::Compact );

  QNetworkReply *reply = mManager.post( request, json );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::createProjectFinished );
  CoreUtils::log( "create " + projectFullName, QStringLiteral( "Requesting project creation: " ) + url.toString() );
}

void MerginApi::deleteProject( const QString &projectNamespace, const QString &projectName, bool informUser )
{
  if ( !validateAuthAndContinute() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QString projectFullName = getFullProjectName( projectNamespace, projectName );

  QNetworkRequest request = getDefaultRequest();
  QUrl url( mApiRoot + QStringLiteral( "/v1/project/%1" ).arg( projectFullName ) );
  request.setUrl( url );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ), projectFullName );
  QNetworkReply *reply = mManager.deleteResource( request );
  connect( reply, &QNetworkReply::finished, this, [this, informUser]() { this->deleteProjectFinished( informUser );} );
  CoreUtils::log( "delete " + projectFullName, QStringLiteral( "Requesting project deletion: " ) + url.toString() );
}

void MerginApi::saveAuthData()
{
  QSettings settings;
  settings.beginGroup( "Input/" );
  settings.setValue( "apiRoot", mApiRoot );
  settings.endGroup();

  mUserAuth->saveAuthData();
}

void MerginApi::createProjectFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "create " + projectFullName, QStringLiteral( "Success" ) );
    emit projectCreated( projectFullName, true );

    QString projectNamespace, projectName;
    extractProjectName( projectFullName, projectNamespace, projectName );

    // Upload data if createProject has been called for a local project with empty namespace (case of migrating a project)
    for ( const LocalProject &info : mLocalProjects.projects() )
    {
      if ( info.projectName == projectName && info.projectNamespace.isEmpty() )
      {
        mLocalProjects.updateNamespace( info.projectDir, projectNamespace );
        emit projectAttachedToMergin( projectFullName );

        QDir projectDir( info.projectDir );
        if ( projectDir.exists() && !projectDir.isEmpty() )
        {
          uploadProject( projectNamespace, projectName, true );
        }
      }
    }
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    QString message = QStringLiteral( "FAILED - %1: %2" ).arg( r->errorString(), serverMsg );
    CoreUtils::log( "create " + projectFullName, message );
    emit projectCreated( projectFullName, false );
    emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: createProject" ) );
  }
  r->deleteLater();
}

void MerginApi::deleteProjectFinished( bool informUser )
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "delete " + projectFullName, QStringLiteral( "Success" ) );

    if ( informUser )
      emit notify( QStringLiteral( "Project deleted" ) );

    emit serverProjectDeleted( projectFullName, true );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    CoreUtils::log( "delete " + projectFullName, QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );
    emit serverProjectDeleted( projectFullName, false );
    emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: deleteProject" ) );
  }
  r->deleteLater();
}

void MerginApi::authorizeFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "auth", QStringLiteral( "Success" ) );
    const QByteArray data = r->readAll();
    QJsonDocument doc = QJsonDocument::fromJson( data );
    if ( doc.isObject() )
    {
      QJsonObject docObj = doc.object();
      mUserAuth->setFromJson( docObj );
    }
    else
    {
      mUserAuth->blockSignals( true );
      mUserAuth->setUsername( QString() ); //clearTokenData emits the authChanged
      mUserAuth->setPassword( QString() ); //clearTokenData emits the authChanged
      mUserAuth->blockSignals( false );

      mUserAuth->clearTokenData();
      emit authFailed();
      CoreUtils::log( "auth", QStringLiteral( "FAILED - invalid JSON response" ) );
      qDebug() << data;
      emit notify( "Internal server error during authorization" );
    }
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    CoreUtils::log( "auth", QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );
    QVariant statusCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    int status = statusCode.toInt();
    if ( status == 401 || status == 400 )
    {
      emit authFailed();
      emit notify( serverMsg );
    }
    else
    {
      emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: authorize" ) );
    }
    mUserAuth->setUsername( QString() );
    mUserAuth->setPassword( QString() );
    mUserAuth->clearTokenData();
  }
  if ( mAuthLoopEvent.isRunning() )
  {
    mAuthLoopEvent.exit();
  }
  r->deleteLater();
}

void MerginApi::registrationFinished( const QString &username, const QString &password )
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "register", QStringLiteral( "Success" ) );
    emit registrationSucceeded();
    QString msg = tr( "Registration successful" );
    emit notify( msg );

    if ( !username.isEmpty() && !password.isEmpty() ) // log in immediately
      authorize( username, password );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    CoreUtils::log( "register", QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );
    QVariant statusCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    int status = statusCode.toInt();
    if ( status == 401 || status == 400 )
    {
      emit registrationFailed();
      emit notify( serverMsg );
    }
    else if ( status == 404 )
    {
      // the self-registration is not allowed on the server
      emit registrationFailed();
      emit notify( tr( "New registrations are not allowed on the selected Mergin server.%1Please check with your administrator." ).arg( "\n" ) );
    }
    else
    {
      emit registrationFailed();
      emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: register" ) );
    }
  }
  r->deleteLater();
}

void MerginApi::pingMerginReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );
  QString apiVersion;
  QString serverMsg;
  bool serverSupportsSubscriptions = false;

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "ping", QStringLiteral( "Success" ) );
    QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      QJsonObject obj = doc.object();
      apiVersion = obj.value( QStringLiteral( "version" ) ).toString();
      serverSupportsSubscriptions = obj.value( QStringLiteral( "subscriptions_enabled" ) ).toBool();
    }
  }
  else
  {
    serverMsg = extractServerErrorMsg( r->readAll() );
    CoreUtils::log( "ping", QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );
  }
  r->deleteLater();
  emit pingMerginFinished( apiVersion, serverSupportsSubscriptions, serverMsg );
}

void MerginApi::onPlanProductIdChanged()
{
  if ( mUserAuth->hasAuthData() )
  {
    getUserInfo();
  }
}

QNetworkReply *MerginApi::getProjectInfo( const QString &projectFullName, bool withoutAuth )
{
  if ( ( !withoutAuth && !validateAuthAndContinute() ) || mApiVersionStatus != MerginApiStatus::OK )
  {
    return nullptr;
  }

  int sinceVersion = -1;
  LocalProject projectInfo = mLocalProjects.projectFromMerginName( projectFullName );
  if ( projectInfo.isValid() )
  {
    // let's also fetch the recent history of diffable files
    // (the "since" is inclusive, so if we are on v2, we want to use since=v3 which will include v2->v3, v3->v4, ...)
    sinceVersion = projectInfo.localVersion + 1;
  }

  QUrlQuery query;
  if ( sinceVersion != -1 )
    query.addQueryItem( QStringLiteral( "since" ), QStringLiteral( "v%1" ).arg( sinceVersion ) );

  QUrl url( mApiRoot + QStringLiteral( "/v1/project/%1" ).arg( projectFullName ) );
  url.setQuery( query );

  QNetworkRequest request = getDefaultRequest( !withoutAuth );
  request.setUrl( url );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ), projectFullName );

  return mManager.get( request );
}

void MerginApi::loadAuthData()
{
  QSettings settings;
  settings.beginGroup( QStringLiteral( "Input/" ) );
  setApiRoot( settings.value( QStringLiteral( "apiRoot" ) ).toString() );
  mUserAuth->loadAuthData();
}

bool MerginApi::validateAuthAndContinute()
{
  if ( !mUserAuth->hasAuthData() )
  {
    emit authRequested();
    return false;
  }

  if ( mUserAuth->authToken().isEmpty() || mUserAuth->tokenExpiration() < QDateTime().currentDateTime().toUTC() )
  {
    authorize( mUserAuth->username(), mUserAuth->password() );
    CoreUtils::log( QStringLiteral( "MerginApi" ), QStringLiteral( "Requesting authorization because of missing or expired token." ) );
    mAuthLoopEvent.exec();
  }
  return true;
}

void MerginApi::checkMerginVersion( QString apiVersion, bool serverSupportsSubscriptions, QString msg )
{
  setApiSupportsSubscriptions( serverSupportsSubscriptions );

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

QString MerginApi::extractServerErrorMsg( const QByteArray &data )
{
  QString serverMsg;
  QJsonDocument doc = QJsonDocument::fromJson( data );
  if ( doc.isObject() )
  {
    QJsonObject obj = doc.object();
    QJsonValue vDetail = obj.value( "detail" );
    if ( vDetail.isString() )
    {
      serverMsg = vDetail.toString();
    }
    else if ( vDetail.isObject() )
    {
      serverMsg = QJsonDocument( vDetail.toObject() ).toJson();
    }
    else
    {
      serverMsg = "[can't parse server error]";
    }
  }
  else
  {
    // take only first 1000 bytes of the message ~ there are situations when data is an unclosed string that would eat the whole log memory
    serverMsg = data.mid( 0, 1000 );
  }

  return serverMsg;
}


LocalProject MerginApi::getLocalProject( const QString &projectFullName )
{
  return mLocalProjects.projectFromMerginName( projectFullName );
}

ProjectDiff MerginApi::localProjectChanges( const QString &projectDir )
{
  MerginProjectMetadata projectMetadata = MerginProjectMetadata::fromCachedJson( projectDir + "/" + sMetadataFile );
  QList<MerginFile> localFiles = getLocalProjectFiles( projectDir + "/" );

  MerginConfig config = MerginConfig::fromFile( projectDir + "/" + sMerginConfigFile );

  return compareProjectFiles( projectMetadata.files, projectMetadata.files, localFiles, projectDir, config.isValid, config );
}

QString MerginApi::getTempProjectDir( const QString &projectFullName )
{
  return mDataDir + "/" + TEMP_FOLDER + projectFullName;
}

QString MerginApi::getFullProjectName( QString projectNamespace, QString projectName ) // TODO: move to inpututils?
{
  return QString( "%1/%2" ).arg( projectNamespace ).arg( projectName );
}

MerginApiStatus::VersionStatus MerginApi::apiVersionStatus() const
{
  return mApiVersionStatus;
}

void MerginApi::setApiVersionStatus( const MerginApiStatus::VersionStatus &apiVersionStatus )
{
  if ( mApiVersionStatus != apiVersionStatus )
  {
    mApiVersionStatus = apiVersionStatus;
    emit apiVersionStatusChanged();
  }
}

void MerginApi::pingMergin()
{
  if ( mApiVersionStatus == MerginApiStatus::OK ) return;

  setApiVersionStatus( MerginApiStatus::PENDING );

  QNetworkRequest request = getDefaultRequest( false );
  QUrl url( mApiRoot + QStringLiteral( "/ping" ) );
  request.setUrl( url );

  QNetworkReply *reply = mManager.get( request );
  CoreUtils::log( "ping", QStringLiteral( "Requesting: " ) + url.toString() );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::pingMerginReplyFinished );
}

void MerginApi::migrateProjectToMergin( const QString &projectName, const QString &projectNamespace )
{
  CoreUtils::log( "migrate project", projectName );
  if ( projectNamespace.isEmpty() )
  {
    createProject( mUserAuth->username(), projectName );
  }
  else
  {
    createProject( projectNamespace, projectName );
  }
}

void MerginApi::detachProjectFromMergin( const QString &projectNamespace, const QString &projectName, bool informUser )
{
  // Remove mergin folder
  QString projectFullName = getFullProjectName( projectNamespace, projectName );
  LocalProject projectInfo = mLocalProjects.projectFromMerginName( projectFullName );

  if ( projectInfo.isValid() )
  {
    CoreUtils::removeDir( projectInfo.projectDir + "/.mergin" );
  }

  // Update localProject
  mLocalProjects.updateNamespace( projectInfo.projectDir, "" );
  mLocalProjects.updateLocalVersion( projectInfo.projectDir, -1 );

  if ( informUser )
    emit notify( tr( "Project detached from Mergin" ) );

  emit projectDetached( projectFullName );
}

QString MerginApi::apiRoot() const
{
  return mApiRoot;
}

void MerginApi::setApiRoot( const QString &apiRoot )
{
  QString newApiRoot;
  if ( apiRoot.isEmpty() )
  {
    newApiRoot = defaultApiRoot();
  }
  else
  {
    newApiRoot = apiRoot;
  }

  if ( newApiRoot  != mApiRoot )
  {
    mApiRoot = newApiRoot;
    QSettings settings;
    settings.beginGroup( QStringLiteral( "Input/" ) );
    settings.setValue( QStringLiteral( "apiRoot" ), mApiRoot );
    settings.endGroup();
    setApiVersionStatus( MerginApiStatus::UNKNOWN );
    emit apiRootChanged();
  }
}

QString MerginApi::merginUserName() const
{
  return userAuth()->username();
}

QList<MerginFile> MerginApi::getLocalProjectFiles( const QString &projectPath )
{
  QList<MerginFile> merginFiles;
  QSet<QString> localFiles = listFiles( projectPath );
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
    merginFiles.append( file );
  }
  return merginFiles;
}

void MerginApi::listProjectsReplyFinished( QString requestId )
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  int projectCount = -1;
  int requestedPage = 1;
  MerginProjectsList projectList;

  if ( r->error() == QNetworkReply::NoError )
  {
    QUrlQuery query( r->request().url().query() );
    requestedPage = query.queryItemValue( "page" ).toInt();

    QByteArray data = r->readAll();
    QJsonDocument doc = QJsonDocument::fromJson( data );

    if ( doc.isObject() )
    {
      projectCount = doc.object().value( "count" ).toInt();
      projectList = parseProjectsFromJson( doc );
    }

    CoreUtils::log( "list projects", QStringLiteral( "Success - got %1 projects" ).arg( projectList.count() ) );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "listProjects" ), r->errorString(), serverMsg );
    emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: listProjects" ) );
    CoreUtils::log( "list projects", QStringLiteral( "FAILED - %1" ).arg( message ) );

    emit listProjectsFailed();
  }

  r->deleteLater();

  emit listProjectsFinished( projectList, mTransactionalStatus, projectCount, requestedPage, requestId );
}

void MerginApi::listProjectsByNameReplyFinished( QString requestId )
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  MerginProjectsList projectList;

  if ( r->error() == QNetworkReply::NoError )
  {
    QByteArray data = r->readAll();
    QJsonDocument json = QJsonDocument::fromJson( data );
    projectList = parseProjectsFromJson( json );
    CoreUtils::log( "list projects by name", QStringLiteral( "Success - got %1 projects" ).arg( projectList.count() ) );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "listProjectsByName" ), r->errorString(), serverMsg );
    emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: listProjectsByName" ) );
    CoreUtils::log( "list projects by name", QStringLiteral( "FAILED - %1" ).arg( message ) );

    emit listProjectsFailed();
  }

  r->deleteLater();

  emit listProjectsByNameFinished( projectList, mTransactionalStatus, requestId );
}


void MerginApi::finalizeProjectUpdateCopy( const QString &projectFullName, const QString &projectDir, const QString &tempDir, const QString &filePath, const QList<DownloadQueueItem> &items )
{
  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Copying new content of " ) + filePath );

  QString dest = projectDir + "/" + filePath;
  createPathIfNotExists( dest );

  QFile f( dest );
  if ( !f.open( QIODevice::WriteOnly ) )
  {
    CoreUtils::log( "pull " + projectFullName, "Failed to open file for writing " + dest );
    return;
  }

  // assemble file from tmp files
  for ( const auto &item : items )
  {
    QFile fTmp( tempDir + "/" + item.tempFileName );
    if ( !fTmp.open( QIODevice::ReadOnly ) )
    {
      CoreUtils::log( "pull " + projectFullName, "Failed to open temp file for reading " + item.tempFileName );
      return;
    }
    f.write( fTmp.readAll() );
  }

  f.close();

  // if diffable, copy to .mergin dir so we have a basefile
  if ( MerginApi::isFileDiffable( filePath ) )
  {
    QString basefile = projectDir + "/.mergin/" + filePath;
    createPathIfNotExists( basefile );

    if ( !QFile::remove( basefile ) )
    {
      CoreUtils::log( "pull " + projectFullName, "failed to remove old basefile for: " + filePath );
    }
    if ( !QFile::copy( dest, basefile ) )
    {
      CoreUtils::log( "pull " + projectFullName, "failed to copy new basefile for: " + filePath );
    }
  }
}


void MerginApi::finalizeProjectUpdateApplyDiff( const QString &projectFullName, const QString &projectDir, const QString &tempDir, const QString &filePath, const QList<DownloadQueueItem> &items )
{
  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Applying diff to " ) + filePath );

  // update diffable files that have been modified on the server
  // - if they were not modified locally, the server changes will be simply applied
  // - if they were modified locally, local changes will be rebased on top of server changes

  QString src = tempDir + "/" + CoreUtils::uuidWithoutBraces( QUuid::createUuid() );
  QString dest = projectDir + "/" + filePath;
  QString basefile = projectDir + "/.mergin/" + filePath;

  LocalProject info = mLocalProjects.projectFromMerginName( projectFullName );

  // add conflict files to project dir so they can be synced
  QString conflictfile = CoreUtils::findUniquePath( CoreUtils::generateEditConflictFileName( dest, mUserAuth->username(), info.localVersion ) );

  createPathIfNotExists( src );
  createPathIfNotExists( dest );
  createPathIfNotExists( basefile );

  QStringList diffFiles;
  for ( const auto &item : items )
    diffFiles << tempDir + "/" + item.tempFileName;

  //
  // let's first assemble server's file from our basefile + diffs
  //

  if ( !QFile::copy( basefile, src ) )
  {
    CoreUtils::log( "pull " + projectFullName, "assemble server file fail: copying failed " + basefile + " to " + src );

    // TODO: this is a critical failure - we should abort pull
  }

  if ( !GeodiffUtils::applyDiffs( src, diffFiles ) )
  {
    CoreUtils::log( "pull " + projectFullName, "server file assembly failed: " + filePath );

    // TODO: this is a critical failure - we should abort pull
    // TODO: we could try to delete the basefile and re-download it from scratch on next sync
  }
  else
  {
    CoreUtils::log( "pull " + projectFullName, "server file assembly successful: " + filePath );
  }

  //
  // now we are ready for the update of our local file
  //

  int res = GEODIFF_rebase( basefile.toUtf8().constData(),
                            src.toUtf8().constData(),
                            dest.toUtf8().constData(),
                            conflictfile.toUtf8().constData()
                          );
  if ( res == GEODIFF_SUCCESS )
  {
    CoreUtils::log( "pull " + projectFullName, "geodiff rebase successful: " + filePath );
  }
  else
  {
    CoreUtils::log( "pull " + projectFullName, "geodiff rebase failed! " + filePath );

    // not good... something went wrong in rebase - we need to save the local changes
    // let's put them into a conflict file and use the server version
    LocalProject info = mLocalProjects.projectFromMerginName( projectFullName );
    QString newDest = CoreUtils::findUniquePath( CoreUtils::generateConflictedCopyFileName( dest, mUserAuth->username(), info.localVersion ) );
    if ( !QFile::rename( dest, newDest ) )
    {
      CoreUtils::log( "pull " + projectFullName, "failed rename of conflicting file after failed geodiff rebase: " + filePath );
    }
    if ( !QFile::copy( src, dest ) )
    {
      CoreUtils::log( "pull " + projectFullName, "failed to update local conflicting file after failed geodiff rebase: " + filePath );
    }
  }

  //
  // finally update our basefile
  //

  if ( !QFile::remove( basefile ) )
  {
    CoreUtils::log( "pull " + projectFullName, "failed removal of old basefile: " + filePath );

    // TODO: this is a critical failure - we should abort pull
  }
  if ( !QFile::rename( src, basefile ) )
  {
    CoreUtils::log( "pull " + projectFullName, "failed rename of basefile using new server content: " + filePath );

    // TODO: this is a critical failure - we should abort pull
  }
}

void MerginApi::finalizeProjectUpdate( const QString &projectFullName )
{
  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];

  QString projectDir = transaction.projectDir;
  QString tempProjectDir = getTempProjectDir( projectFullName );

  CoreUtils::log( "pull " + projectFullName, "Running update tasks" );

  for ( const UpdateTask &finalizationItem : transaction.updateTasks )
  {
    switch ( finalizationItem.method )
    {
      case UpdateTask::Copy:
      {
        finalizeProjectUpdateCopy( projectFullName, projectDir, tempProjectDir, finalizationItem.filePath, finalizationItem.data );
        break;
      }

      case UpdateTask::CopyConflict:
      {
        // move local file to conflict file
        QString origPath = projectDir + "/" + finalizationItem.filePath;
        LocalProject info = mLocalProjects.projectFromMerginName( projectFullName );
        QString newPath = CoreUtils::findUniquePath( CoreUtils::generateConflictedCopyFileName( origPath, mUserAuth->username(), info.localVersion ) );
        if ( !QFile::rename( origPath, newPath ) )
        {
          CoreUtils::log( "pull " + projectFullName, "failed rename of conflicting file: " + finalizationItem.filePath );
        }
        else
        {
          CoreUtils::log( "pull " + projectFullName, "Local file renamed due to conflict with server: " + finalizationItem.filePath );
        }
        finalizeProjectUpdateCopy( projectFullName, projectDir, tempProjectDir, finalizationItem.filePath, finalizationItem.data );
        break;
      }

      case UpdateTask::ApplyDiff:
      {
        finalizeProjectUpdateApplyDiff( projectFullName, projectDir, tempProjectDir, finalizationItem.filePath, finalizationItem.data );
        break;
      }

      case UpdateTask::Delete:
      {
        CoreUtils::log( "pull " + projectFullName, "Removing local file: " + finalizationItem.filePath );
        QFile file( projectDir + "/" + finalizationItem.filePath );
        file.remove();
        break;
      }
    }

    // remove tmp files associated with this item
    for ( const auto &downloadItem : finalizationItem.data )
    {
      if ( !QFile::remove( tempProjectDir + "/" + downloadItem.tempFileName ) )
        CoreUtils::log( "pull " + projectFullName, "Failed to remove temporary file " + downloadItem.tempFileName );
    }
  }

  // check there are no files left
  int tmpFilesLeft = QDir( tempProjectDir ).entryList( QDir::NoDotAndDotDot ).count();
  if ( tmpFilesLeft )
  {
    CoreUtils::log( "pull " + projectFullName, "Some temporary files were left - this should not happen..." );
  }

  QDir( tempProjectDir ).removeRecursively();

  // add the local project if not there yet
  if ( !mLocalProjects.projectFromMerginName( projectFullName ).isValid() )
  {
    QString projectNamespace, projectName;
    extractProjectName( projectFullName, projectNamespace, projectName );

    // remove download in progress file
    if ( !QFile::remove( CoreUtils::downloadInProgressFilePath( transaction.projectDir ) ) )
      CoreUtils::log( QStringLiteral( "sync %1" ).arg( projectFullName ), QStringLiteral( "Failed to remove download in progress file for project name %1" ).arg( projectName ) );

    mLocalProjects.addMerginProject( projectDir, projectNamespace, projectName );
  }

  finishProjectSync( projectFullName, true );
}


void MerginApi::uploadStartReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();

  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];
  Q_ASSERT( r == transaction.replyUploadStart );

  if ( r->error() == QNetworkReply::NoError )
  {
    QByteArray data = r->readAll();

    transaction.replyUploadStart->deleteLater();
    transaction.replyUploadStart = nullptr;

    QList<MerginFile> files = transaction.uploadQueue;
    if ( !files.isEmpty() )
    {
      QString transactionUUID;
      QJsonDocument doc = QJsonDocument::fromJson( data );
      if ( doc.isObject() )
      {
        QJsonObject docObj = doc.object();
        transactionUUID = docObj.value( QStringLiteral( "transaction" ) ).toString();
        transaction.transactionUUID = transactionUUID;
      }

      CoreUtils::log( "push " + projectFullName, QStringLiteral( "Push request accepted. Transaction ID: " ) + transactionUUID );

      MerginFile file = files.first();
      uploadFile( projectFullName, transactionUUID, file );
      emit pushFilesStarted();
    }
    else  // pushing only files to be removed
    {
      // we are done here - no upload of chunks, no request to "finish"
      // because server immediatelly creates a new version without starting a transaction to upload chunks

      CoreUtils::log( "push " + projectFullName, QStringLiteral( "Push request accepted and no files to upload" ) );

      transaction.projectMetadata = data;
      transaction.version = MerginProjectMetadata::fromJson( data ).version;

      finishProjectSync( projectFullName, true );
    }
  }
  else
  {
    QVariant statusCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    int status = statusCode.toInt();
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    QString errorMsg = r->errorString();
    bool showLimitReachedDialog = status == 400 && serverMsg.contains( QStringLiteral( "You have reached a data limit" ) );

    CoreUtils::log( "push " + projectFullName, QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );

    transaction.replyUploadStart->deleteLater();
    transaction.replyUploadStart = nullptr;

    if ( showLimitReachedDialog )
    {
      const QList<MerginFile> files = transaction.uploadQueue;
      qreal uploadSize = 0;
      for ( const MerginFile &f : files )
      {
        uploadSize += f.size;
      }
      emit storageLimitReached( uploadSize );

      // remove project if it was first time sync - migration
      if ( transaction.isInitialUpload )
      {
        QString projectNamespace, projectName;
        extractProjectName( projectFullName, projectNamespace, projectName );

        detachProjectFromMergin( projectNamespace, projectName, false );
        deleteProject( projectNamespace, projectName, false );
      }
    }
    else
    {
      emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: uploadStartReply" ) );
    }

    finishProjectSync( projectFullName, false );
  }
}

void MerginApi::uploadFileReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();

  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];
  Q_ASSERT( r == transaction.replyUploadFile );

  QStringList params = ( r->url().toString().split( "/" ) );
  QString transactionUUID = params.at( params.length() - 2 );
  QString chunkID = params.at( params.length() - 1 );
  Q_ASSERT( transactionUUID == transaction.transactionUUID );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Uploaded successfully: " ) + chunkID );

    transaction.replyUploadFile->deleteLater();
    transaction.replyUploadFile = nullptr;

    MerginFile currentFile = transaction.uploadQueue.first();
    int chunkNo = currentFile.chunks.indexOf( chunkID );
    if ( chunkNo < currentFile.chunks.size() - 1 )
    {
      uploadFile( projectFullName, transactionUUID, currentFile, chunkNo + 1 );
    }
    else
    {
      transaction.transferedSize += currentFile.size;

      emit syncProjectStatusChanged( projectFullName, transaction.transferedSize / transaction.totalSize );
      transaction.uploadQueue.removeFirst();

      if ( !transaction.uploadQueue.isEmpty() )
      {
        MerginFile nextFile = transaction.uploadQueue.first();
        uploadFile( projectFullName, transactionUUID, nextFile );
      }
      else
      {
        uploadFinish( projectFullName, transactionUUID );
      }
    }
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );
    emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: uploadFile" ) );

    transaction.replyUploadFile->deleteLater();
    transaction.replyUploadFile = nullptr;

    finishProjectSync( projectFullName, false );
  }
}

void MerginApi::updateInfoReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();

  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];
  Q_ASSERT( r == transaction.replyProjectInfo );

  if ( r->error() == QNetworkReply::NoError )
  {
    QByteArray data = r->readAll();
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Downloaded project info." ) );

    transaction.replyProjectInfo->deleteLater();
    transaction.replyProjectInfo = nullptr;

    prepareProjectUpdate( projectFullName, data );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    QString message = QStringLiteral( "Network API error: %1(): %2" ).arg( QStringLiteral( "projectInfo" ), r->errorString() );
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "FAILED - %1" ).arg( message ) );
    emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: updateInfo" ) );

    transaction.replyProjectInfo->deleteLater();
    transaction.replyProjectInfo = nullptr;

    finishProjectSync( projectFullName, false );
  }
}

void MerginApi::prepareProjectUpdate( const QString &projectFullName, const QByteArray &data )
{
  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];

  LocalProject projectInfo = mLocalProjects.projectFromMerginName( projectFullName );
  if ( projectInfo.isValid() )
  {
    transaction.projectDir = projectInfo.projectDir;
  }
  else
  {
    QString projectNamespace;
    QString projectName;
    extractProjectName( projectFullName, projectNamespace, projectName );

    // remove any leftover temp files that could be created from previous unsuccessful download
    removeProjectsTempFolder( projectNamespace, projectName );

    // project has not been downloaded yet - we need to create a directory for it
    transaction.projectDir = CoreUtils::createUniqueProjectDirectory( mDataDir, projectName );
    transaction.firstTimeDownload = true;

    // create file indicating first time download in progress
    QString downloadInProgressFilePath = CoreUtils::downloadInProgressFilePath( transaction.projectDir );
    createPathIfNotExists( downloadInProgressFilePath );
    if ( !CoreUtils::createEmptyFile( downloadInProgressFilePath ) )
      CoreUtils::log( QStringLiteral( "pull %1" ).arg( projectFullName ), "Unable to create temporary download in progress file" );

    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "First time download - new directory: " ) + transaction.projectDir );
  }

  Q_ASSERT( !transaction.projectDir.isEmpty() );  // that would mean we do not have entry -> fail getting local files

  MerginProjectMetadata serverProject = MerginProjectMetadata::fromJson( data );
  MerginProjectMetadata oldServerProject = MerginProjectMetadata::fromCachedJson( transaction.projectDir + "/" + sMetadataFile );

  transaction.projectMetadata = data;
  transaction.version = serverProject.version;

  if ( transaction.configAllowed )
  {
    prepareDownloadConfig( projectFullName );
  }
  else
  {
    startProjectUpdate( projectFullName );
  }
}

void MerginApi::startProjectUpdate( const QString &projectFullName )
{
  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];

  QList<MerginFile> localFiles = getLocalProjectFiles( transaction.projectDir + "/" );
  MerginProjectMetadata serverProject = MerginProjectMetadata::fromJson( transaction.projectMetadata );
  MerginProjectMetadata oldServerProject = MerginProjectMetadata::fromCachedJson( transaction.projectDir + "/" + sMetadataFile );
  MerginConfig oldTransactionConfig = MerginConfig::fromFile( transaction.projectDir + "/" + sMerginConfigFile );

  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Updating from version %1 to version %2" )
                  .arg( oldServerProject.version ).arg( serverProject.version ) );

  transaction.diff = compareProjectFiles(
                       oldServerProject.files,
                       serverProject.files,
                       localFiles,
                       transaction.projectDir,
                       transaction.configAllowed,
                       transaction.config,
                       oldTransactionConfig );

  CoreUtils::log( "pull " + projectFullName, transaction.diff.dump() );

  for ( QString filePath : transaction.diff.remoteAdded )
  {
    MerginFile file = serverProject.fileInfo( filePath );
    QList<DownloadQueueItem> items = itemsForFileChunks( file, transaction.version );
    transaction.updateTasks << UpdateTask( UpdateTask::Copy, filePath, items );
  }

  for ( QString filePath : transaction.diff.remoteUpdated )
  {
    MerginFile file = serverProject.fileInfo( filePath );

    // for diffable files - download and apply to the basefile (without rebase)
    if ( isFileDiffable( filePath ) && file.pullCanUseDiff )
    {
      QList<DownloadQueueItem> items = itemsForFileDiffs( file );
      transaction.updateTasks << UpdateTask( UpdateTask::ApplyDiff, filePath, items );
    }
    else
    {
      QList<DownloadQueueItem> items = itemsForFileChunks( file, transaction.version );
      transaction.updateTasks << UpdateTask( UpdateTask::Copy, filePath, items );
    }
  }

  // also download files which were changed both on the server and locally (the local version will be renamed as conflicting copy)
  for ( QString filePath : transaction.diff.conflictRemoteUpdatedLocalUpdated )
  {
    MerginFile file = serverProject.fileInfo( filePath );

    // for diffable files - download and apply to the basefile (will also do rebase)
    if ( isFileDiffable( filePath ) && file.pullCanUseDiff )
    {
      QList<DownloadQueueItem> items = itemsForFileDiffs( file );
      transaction.updateTasks << UpdateTask( UpdateTask::ApplyDiff, filePath, items );
    }
    else
    {
      QList<DownloadQueueItem> items = itemsForFileChunks( file, transaction.version );
      transaction.updateTasks << UpdateTask( UpdateTask::CopyConflict, filePath, items );
    }
  }

  // also download files which were added both on the server and locally (the local version will be renamed as conflicting copy)
  for ( QString filePath : transaction.diff.conflictRemoteAddedLocalAdded )
  {
    MerginFile file = serverProject.fileInfo( filePath );
    QList<DownloadQueueItem> items = itemsForFileChunks( file, transaction.version );
    transaction.updateTasks << UpdateTask( UpdateTask::CopyConflict, filePath, items );
  }

  // schedule removed files to be deleted
  for ( QString filePath : transaction.diff.remoteDeleted )
  {
    transaction.updateTasks << UpdateTask( UpdateTask::Delete, filePath, QList<DownloadQueueItem>() );
  }

  // prepare the download queue
  for ( const UpdateTask &item : transaction.updateTasks )
  {
    transaction.downloadQueue << item.data;
  }

  qint64 totalSize = 0;
  for ( const DownloadQueueItem &item : transaction.downloadQueue )
  {
    totalSize += item.size;
  }
  transaction.totalSize = totalSize;

  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "%1 update tasks, %2 items to download (total size %3 bytes)" )
                  .arg( transaction.updateTasks.count() )
                  .arg( transaction.downloadQueue.count() )
                  .arg( transaction.totalSize ) );

  emit pullFilesStarted();
  downloadNextItem( projectFullName );
}

void MerginApi::prepareDownloadConfig( const QString &projectFullName, bool downloaded )
{
  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];

  MerginProjectMetadata newServerVersion = MerginProjectMetadata::fromJson( transaction.projectMetadata );

  const auto res = std::find_if( newServerVersion.files.begin(), newServerVersion.files.end(), []( const MerginFile & file )
  {
    return file.path == sMerginConfigFile;
  } );
  bool serverContainsConfig = res != newServerVersion.files.end();

  if ( serverContainsConfig )
  {
    if ( !downloaded )
    {
      // we should have server config but we do not have it yet
      return requestServerConfig( projectFullName );
    }
  }

  MerginProjectMetadata oldServerVersion = MerginProjectMetadata::fromCachedJson( transaction.projectDir + "/" + sMetadataFile );

  const auto resOld = std::find_if( oldServerVersion.files.begin(), oldServerVersion.files.end(), []( const MerginFile & file )
  {
    return file.path == sMerginConfigFile;
  } );

  bool previousVersionContainedConfig = ( resOld != oldServerVersion.files.end() ) && !transaction.firstTimeDownload;

  if ( !transaction.config.isValid )
  {
    // if transaction is not valid (or missing), consider it as deleted
    transaction.config.downloadMissingFiles = true;
    CoreUtils::log( "MerginConfig", "No config detected" );
  }
  else if ( serverContainsConfig && previousVersionContainedConfig )
  {
    // config was there, check if there are changes
    QString newChk = newServerVersion.fileInfo( sMerginConfigFile ).checksum;
    QString oldChk = oldServerVersion.fileInfo( sMerginConfigFile ).checksum;

    if ( newChk == oldChk )
    {
      // config files are the same
    }
    else
    {
      // config was changed, but what changed?
      MerginConfig oldConfig = MerginConfig::fromFile( transaction.projectDir + "/" + MerginApi::sMerginConfigFile );

      if ( oldConfig.selectiveSyncEnabled != transaction.config.selectiveSyncEnabled )
      {
        // selective sync was enabled/disabled
        if ( transaction.config.selectiveSyncEnabled )
        {
          CoreUtils::log( "MerginConfig", "Selective sync has been enabled" );
        }
        else
        {
          CoreUtils::log( "MerginConfig", "Selective sync has been disabled, downloading missing files." );
          transaction.config.downloadMissingFiles = true;
        }
      }
      else if ( oldConfig.selectiveSyncDir != transaction.config.selectiveSyncDir )
      {
        CoreUtils::log( "MerginConfig", "Selective sync directory has changed, downloading missing files." );
        transaction.config.downloadMissingFiles = true;
      }
      else
      {
        CoreUtils::log( "MerginConfig", "Unknown change in config file, continuing with latest version." );
      }
    }
  }
  else if ( serverContainsConfig )
  {
    CoreUtils::log( "MerginConfig", "Detected new config file." );
  }
  else if ( previousVersionContainedConfig ) // and current does not
  {
    CoreUtils::log( "MerginConfig", "Config file was removed, downloading missing files." );
    transaction.config.downloadMissingFiles = true;
  }
  else // no config in last versions
  {
    // update like without config
    transaction.configAllowed = false;
    transaction.config.isValid = false;

    // if it would be possible to add mergin-config locally, it needs to be checked here
  }

  startProjectUpdate( projectFullName );
}

void MerginApi::requestServerConfig( const QString &projectFullName )
{
  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];

  QUrl url( mApiRoot + QStringLiteral( "/v1/project/raw/" ) + projectFullName );
  QUrlQuery query;

  query.addQueryItem( "file", sMerginConfigFile.toUtf8().toPercentEncoding() );
  query.addQueryItem( "version", QStringLiteral( "v%1" ).arg( transaction.version ) );
  url.setQuery( query );

  QNetworkRequest request = getDefaultRequest();
  request.setUrl( url );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ), projectFullName );

  Q_ASSERT( !transaction.replyDownloadItem );
  transaction.replyDownloadItem = mManager.get( request );
  connect( transaction.replyDownloadItem, &QNetworkReply::finished, this, &MerginApi::cacheServerConfig );

  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Requesting mergin config: " ) + url.toString() );
}

QList<DownloadQueueItem> MerginApi::itemsForFileChunks( const MerginFile &file, int version )
{
  QList<DownloadQueueItem> lst;
  int from = 0;
  while ( from < file.size )
  {
    int size = qMin( MerginApi::UPLOAD_CHUNK_SIZE, static_cast<int>( file.size ) - from );
    lst << DownloadQueueItem( file.path, size, version, from, from + size - 1 );
    from += size;
  }
  return lst;
}

QList<DownloadQueueItem> MerginApi::itemsForFileDiffs( const MerginFile &file )
{
  QList<DownloadQueueItem> items;
  // download diffs instead of full download of gpkg file from server
  for ( const auto &d : file.pullDiffFiles )
  {
    items << DownloadQueueItem( file.path, d.second, d.first, -1, -1, true );
  }
  return items;
}


static MerginFile findFile( const QString &filePath, const QList<MerginFile> &files )
{
  for ( const MerginFile &merginFile : files )
  {
    if ( merginFile.path == filePath )
      return merginFile;
  }
  qDebug() << "requested findFile() for non-existant file! " << filePath;
  return MerginFile();
}


void MerginApi::uploadInfoReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();

  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];
  Q_ASSERT( r == transaction.replyUploadProjectInfo );

  if ( r->error() == QNetworkReply::NoError )
  {
    QString url = r->url().toString();
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Downloaded project info." ) );
    QByteArray data = r->readAll();

    transaction.replyUploadProjectInfo->deleteLater();
    transaction.replyUploadProjectInfo = nullptr;

    LocalProject projectInfo = mLocalProjects.projectFromMerginName( projectFullName );
    transaction.projectDir = projectInfo.projectDir;
    Q_ASSERT( !transaction.projectDir.isEmpty() );

    // get the latest server version from our reply (we do not update it in LocalProjectsManager though... I guess we don't need to)
    MerginProjectMetadata serverProject = MerginProjectMetadata::fromJson( data );

    // now let's figure a key question: are we on the most recent version of the project
    // if we're about to do upload? because if not, we need to do local update first
    if ( projectInfo.isValid() && projectInfo.localVersion != -1 && projectInfo.localVersion < serverProject.version )
    {
      CoreUtils::log( "push " + projectFullName, QStringLiteral( "Need pull first: local version %1 | server version %2" )
                      .arg( projectInfo.localVersion ).arg( serverProject.version ) );
      transaction.updateBeforeUpload = true;
      prepareProjectUpdate( projectFullName, data );
      return;
    }

    QList<MerginFile> localFiles = getLocalProjectFiles( transaction.projectDir + "/" );
    MerginProjectMetadata oldServerProject = MerginProjectMetadata::fromCachedJson( transaction.projectDir + "/" + sMetadataFile );

    // Cache mergin-config, since we are on the most recent version, it is sufficient to just read the local version
    if ( transaction.configAllowed )
    {
      transaction.config = MerginConfig::fromFile( transaction.projectDir + "/" + MerginApi::sMerginConfigFile );
    }

    transaction.diff = compareProjectFiles(
                         oldServerProject.files,
                         serverProject.files,
                         localFiles,
                         transaction.projectDir,
                         transaction.configAllowed,
                         transaction.config
                       );

    CoreUtils::log( "push " + projectFullName, transaction.diff.dump() );

    // TODO: make sure there are no remote files to add/update/remove nor conflicts

    QList<MerginFile> filesToUpload;
    QList<MerginFile> addedMerginFiles, updatedMerginFiles, deletedMerginFiles;
    QList<MerginFile> diffFiles;
    for ( QString filePath : transaction.diff.localAdded )
    {
      MerginFile merginFile = findFile( filePath, localFiles );
      merginFile.chunks = generateChunkIdsForSize( merginFile.size );
      addedMerginFiles.append( merginFile );
    }

    for ( QString filePath : transaction.diff.localUpdated )
    {
      MerginFile merginFile = findFile( filePath, localFiles );
      merginFile.chunks = generateChunkIdsForSize( merginFile.size );

      if ( MerginApi::isFileDiffable( filePath ) )
      {
        // try to create a diff
        QString diffName;
        int geodiffRes = GeodiffUtils::createChangeset( transaction.projectDir, filePath, diffName );
        QString diffPath = transaction.projectDir + "/.mergin/" + diffName;
        QString basePath = transaction.projectDir + "/.mergin/" + filePath;

        if ( geodiffRes == GEODIFF_SUCCESS )
        {
          QByteArray checksumDiff = getChecksum( diffPath );

          // TODO: this is ugly. our basefile may not need to have the same checksum as the server's
          // basefile (because each of them have applied the diff independently) so we have to fake it
          QByteArray checksumBase = serverProject.fileInfo( filePath ).checksum.toLatin1();

          merginFile.diffName = diffName;
          merginFile.diffChecksum = QString::fromLatin1( checksumDiff.data(), checksumDiff.size() );
          merginFile.diffSize = QFileInfo( diffPath ).size();
          merginFile.chunks = generateChunkIdsForSize( merginFile.diffSize );
          merginFile.diffBaseChecksum = QString::fromLatin1( checksumBase.data(), checksumBase.size() );

          diffFiles.append( merginFile );

          CoreUtils::log( "push " + projectFullName, QString( "Geodiff create changeset on %1 successful: total size %2 bytes" ).arg( filePath ).arg( merginFile.diffSize ) );
        }
        else
        {
          // TODO: remove the diff file (if exists)
          CoreUtils::log( "push " + projectFullName, QString( "Geodiff create changeset on %1 FAILED with error %2 (will do full upload)" ).arg( filePath ).arg( geodiffRes ) );
        }
      }

      updatedMerginFiles.append( merginFile );
    }

    for ( QString filePath : transaction.diff.localDeleted )
    {
      MerginFile merginFile = findFile( filePath, serverProject.files );
      deletedMerginFiles.append( merginFile );
    }

    if ( addedMerginFiles.isEmpty() && updatedMerginFiles.isEmpty() && deletedMerginFiles.isEmpty() )
    {
      // if nothing has changed, there is no point to even start upload transaction
      transaction.projectMetadata = data;
      transaction.version = MerginProjectMetadata::fromJson( data ).version;

      finishProjectSync( projectFullName, true );
      return;
    }

    QJsonArray added = prepareUploadChangesJSON( addedMerginFiles );
    filesToUpload.append( addedMerginFiles );

    QJsonArray modified = prepareUploadChangesJSON( updatedMerginFiles );
    filesToUpload.append( updatedMerginFiles );

    QJsonArray removed = prepareUploadChangesJSON( deletedMerginFiles );
    // removed not in filesToUpload

    QJsonObject changes;
    changes.insert( "added", added );
    changes.insert( "removed", removed );
    changes.insert( "updated", modified );
    changes.insert( "renamed", QJsonArray() );

    qint64 totalSize = 0;
    for ( MerginFile file : filesToUpload )
    {
      if ( !file.diffName.isEmpty() )
        totalSize += file.diffSize;
      else
        totalSize += file.size;
    }

    CoreUtils::log( "push " + projectFullName, QStringLiteral( "%1 items to upload (total size %2 bytes)" )
                    .arg( filesToUpload.count() ).arg( totalSize ) );

    transaction.totalSize = totalSize;
    transaction.uploadQueue = filesToUpload;
    transaction.uploadDiffFiles = diffFiles;

    QJsonObject json;
    json.insert( QStringLiteral( "changes" ), changes );
    json.insert( QStringLiteral( "version" ), QString( "v%1" ).arg( serverProject.version ) );
    QJsonDocument jsonDoc;
    jsonDoc.setObject( json );

    uploadStart( projectFullName, jsonDoc.toJson( QJsonDocument::Compact ) );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    QString message = QStringLiteral( "Network API error: %1(): %2" ).arg( QStringLiteral( "projectInfo" ), r->errorString() );
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "FAILED - %1" ).arg( message ) );
    emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: uploadInfo" ) );

    transaction.replyUploadProjectInfo->deleteLater();
    transaction.replyUploadProjectInfo = nullptr;

    finishProjectSync( projectFullName, false );
  }
}

void MerginApi::uploadFinishReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();

  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];
  Q_ASSERT( r == transaction.replyUploadFinish );

  if ( r->error() == QNetworkReply::NoError )
  {
    Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
    QByteArray data = r->readAll();
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Transaction finish accepted" ) );

    transaction.replyUploadFinish->deleteLater();
    transaction.replyUploadFinish = nullptr;

    transaction.projectMetadata = data;
    transaction.version = MerginProjectMetadata::fromJson( data ).version;

    //  a new diffable files suppose to have their basefile copies in .mergin
    for ( QString filePath : transaction.diff.localAdded )
    {
      if ( MerginApi::isFileDiffable( filePath ) )
      {
        QString basefile = transaction.projectDir + "/.mergin/" + filePath;
        createPathIfNotExists( basefile );

        QString sourcePath = transaction.projectDir + "/" + filePath;
        if ( !QFile::copy( sourcePath, basefile ) )
        {
          CoreUtils::log( "push " + projectFullName, "failed to copy new basefile for: " + filePath );
        }
      }
    }

    // clean up diff-related files
    const auto diffFiles = transaction.uploadDiffFiles;
    for ( const MerginFile &merginFile : diffFiles )
    {
      QString diffPath = transaction.projectDir + "/.mergin/" + merginFile.diffName;

      // update basefile (unmodified file that should be equivalent to the server)
      QString basePath = transaction.projectDir + "/.mergin/" + merginFile.path;
      int res = GEODIFF_applyChangeset( basePath.toUtf8(), diffPath.toUtf8() );
      if ( res == GEODIFF_SUCCESS )
      {
        CoreUtils::log( "push " + projectFullName, QString( "Applied %1 to base file of %2" ).arg( merginFile.diffName, merginFile.path ) );
      }
      else
      {
        CoreUtils::log( "push " + projectFullName, QString( "Failed to apply changeset %1 to basefile %2 - error %3" ).arg( diffPath ).arg( basePath ).arg( res ) );
      }

      // remove temporary diff files
      if ( !QFile::remove( diffPath ) )
        CoreUtils::log( "push " + projectFullName, "Failed to remove diff: " + diffPath );
    }

    finishProjectSync( projectFullName, true );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "uploadFinish" ), r->errorString(), serverMsg );
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "FAILED - %1" ).arg( message ) );
    emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: uploadFinish" ) );

    // remove temporary diff files
    const auto diffFiles = transaction.uploadDiffFiles;
    for ( const MerginFile &merginFile : diffFiles )
    {
      QString diffPath = transaction.projectDir + "/.mergin/" + merginFile.diffName;
      if ( !QFile::remove( diffPath ) )
        CoreUtils::log( "push " + projectFullName, "Failed to remove diff: " + diffPath );
    }

    transaction.replyUploadFinish->deleteLater();
    transaction.replyUploadFinish = nullptr;

    finishProjectSync( projectFullName, false );
  }
}

void MerginApi::uploadCancelReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Transaction canceled" ) );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "uploadCancel" ), r->errorString(), serverMsg );
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "FAILED - %1" ).arg( message ) );
  }

  emit uploadCanceled( projectFullName, r->error() == QNetworkReply::NoError );

  r->deleteLater();
}

void MerginApi::getUserInfoFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "user info", QStringLiteral( "Success" ) );
    QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      QJsonObject docObj = doc.object();
      mUserInfo->setFromJson( docObj );
    }
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "getUserInfo" ), r->errorString(), serverMsg );
    CoreUtils::log( "user info", QStringLiteral( "FAILED - %1" ).arg( message ) );
    mUserInfo->clear();
    emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: getUserInfo" ) );
  }

  r->deleteLater();
}

void MerginApi::getSubscriptionInfoFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "subscription info", QStringLiteral( "Success" ) );
    QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      QJsonObject docObj = doc.object();
      mSubscriptionInfo->setFromJson( docObj );
    }
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "getSubscriptionInfo" ), r->errorString(), serverMsg );
    CoreUtils::log( "subscription info", QStringLiteral( "FAILED - %1" ).arg( message ) );
    mSubscriptionInfo->clear();
    emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: getSubscriptionInfo" ) );
  }

  r->deleteLater();
}

ProjectDiff MerginApi::compareProjectFiles(
  const QList<MerginFile> &oldServerFiles,
  const QList<MerginFile> &newServerFiles,
  const QList<MerginFile> &localFiles,
  const QString &projectDir,
  bool allowConfig,
  const MerginConfig &config,
  const MerginConfig &lastSyncConfig
)
{
  ProjectDiff diff;
  QHash<QString, MerginFile> oldServerFilesMap, newServerFilesMap;

  for ( MerginFile file : newServerFiles )
  {
    newServerFilesMap.insert( file.path, file );
  }
  for ( MerginFile file : oldServerFiles )
  {
    oldServerFilesMap.insert( file.path, file );
  }

  for ( MerginFile localFile : localFiles )
  {
    QString filePath = localFile.path;
    bool hasOldServer = oldServerFilesMap.contains( localFile.path );
    bool hasNewServer = newServerFilesMap.contains( localFile.path );
    QString chkOld = oldServerFilesMap.value( localFile.path ).checksum;
    QString chkNew = newServerFilesMap.value( localFile.path ).checksum;
    QString chkLocal = localFile.checksum;

    if ( !hasOldServer && !hasNewServer )
    {
      // L-A
      diff.localAdded << filePath;
    }
    else if ( hasOldServer && !hasNewServer )
    {
      if ( chkOld == chkLocal )
      {
        // R-D
        diff.remoteDeleted << filePath;
      }
      else
      {
        // C/R-D/L-U
        diff.conflictRemoteDeletedLocalUpdated << filePath;
      }
    }
    else if ( !hasOldServer && hasNewServer )
    {
      if ( chkNew != chkLocal )
      {
        // C/R-A/L-A
        diff.conflictRemoteAddedLocalAdded << filePath;
      }
      else
      {
        // R-A/L-A
        // TODO: need to do anything?
      }
    }
    else if ( hasOldServer && hasNewServer )
    {
      // file has already existed
      if ( chkOld == chkNew )
      {
        if ( chkNew != chkLocal )
        {
          // L-U
          if ( isFileDiffable( filePath ) )
          {
            // we need to do a diff here to figure out whether the file is actually changed or not
            // because the real content may be the same although the checksums do not match
            if ( GeodiffUtils::hasPendingChanges( projectDir, filePath ) )
              diff.localUpdated << filePath;
          }
          else
            diff.localUpdated << filePath;
        }
        else
        {
          // no change :-)
        }
      }
      else   // v1 != v2
      {
        if ( chkNew != chkLocal && chkOld != chkLocal )
        {
          // C/R-U/L-U
          if ( isFileDiffable( filePath ) )
          {
            // we need to do a diff here to figure out whether the file is actually changed or not
            // because the real content may be the same although the checksums do not match
            if ( GeodiffUtils::hasPendingChanges( projectDir, filePath ) )
              diff.conflictRemoteUpdatedLocalUpdated << filePath;
            else
              diff.remoteUpdated << filePath;
          }
          else
            diff.conflictRemoteUpdatedLocalUpdated << filePath;
        }
        else if ( chkNew != chkLocal )  // && old == local
        {
          // R-U
          diff.remoteUpdated << filePath;
        }
        else if ( chkOld != chkLocal )  // && new == local
        {
          // R-U/L-U
          // TODO: need to do anything?
        }
        else
          Q_ASSERT( false );   // impossible - should be handled already
      }
    }

    if ( hasOldServer )
      oldServerFilesMap.remove( filePath );
    if ( hasNewServer )
      newServerFilesMap.remove( filePath );
  }

  // go through files listed on the server, but not available locally
  for ( MerginFile file : newServerFilesMap )
  {
    bool hasOldServer = oldServerFilesMap.contains( file.path );

    if ( hasOldServer )
    {
      if ( oldServerFilesMap.value( file.path ).checksum == file.checksum )
      {
        // L-D
        if ( allowConfig )
        {
          bool shouldBeExcludedFromSync = MerginApi::excludeFromSync( file.path, config );
          if ( shouldBeExcludedFromSync )
          {
            continue;
          }

          // check if we should download missing files that were previously ignored (e.g. selective sync has been disabled)
          bool previouslyIgnoredButShouldDownload = \
              config.downloadMissingFiles &&
              lastSyncConfig.isValid &&
              MerginApi::excludeFromSync( file.path, lastSyncConfig );

          if ( previouslyIgnoredButShouldDownload )
          {
            diff.remoteAdded << file.path;
            continue;
          }
        }
        diff.localDeleted << file.path;
      }
      else
      {
        // C/R-U/L-D
        diff.conflictRemoteUpdatedLocalDeleted << file.path;
      }
    }
    else
    {
      // R-A
      if ( allowConfig )
      {
        if ( MerginApi::excludeFromSync( file.path, config ) )
        {
          continue;
        }
      }
      diff.remoteAdded << file.path;
    }

    if ( hasOldServer )
      oldServerFilesMap.remove( file.path );
  }

  for ( MerginFile file : oldServerFilesMap )
  {
    // R-D/L-D
    // TODO: need to do anything?
  }

  return diff;
}

MerginProject MerginApi::parseProjectMetadata( const QJsonObject &proj )
{
  MerginProject project;

  if ( proj.isEmpty() )
  {
    return project;
  }

  if ( proj.contains( QStringLiteral( "error" ) ) )
  {
    // handle project error (user might be logged out / do not have write rights / project is on different server / project is orphaned)
    project.remoteError = proj.value( QStringLiteral( "error" ) ).toInt( 0 ); // error code
    return project;
  }

  project.projectName = proj.value( QStringLiteral( "name" ) ).toString();
  project.projectNamespace = proj.value( QStringLiteral( "namespace" ) ).toString();

  QString versionStr = proj.value( QStringLiteral( "version" ) ).toString();
  if ( versionStr.isEmpty() )
  {
    project.serverVersion = 0;
  }
  else if ( versionStr.startsWith( "v" ) ) // cut off 'v' part from v123
  {
    versionStr = versionStr.mid( 1 );
    project.serverVersion = versionStr.toInt();
  }

  QDateTime updated = QDateTime::fromString( proj.value( QStringLiteral( "updated" ) ).toString(), Qt::ISODateWithMs ).toUTC();
  if ( !updated.isValid() )
  {
    project.serverUpdated = QDateTime::fromString( proj.value( QStringLiteral( "created" ) ).toString(), Qt::ISODateWithMs ).toUTC();
  }
  else
  {
    project.serverUpdated = updated;
  }
  return project;
}


MerginProjectsList MerginApi::parseProjectsFromJson( const QJsonDocument &doc )
{
  if ( !doc.isObject() )
    return MerginProjectsList();

  QJsonObject object = doc.object();
  MerginProjectsList result;

  if ( object.contains( "projects" ) && object.value( "projects" ).isArray() ) // listProjects API
  {
    QJsonArray vArray = object.value( "projects" ).toArray();

    for ( auto it = vArray.constBegin(); it != vArray.constEnd(); ++it )
    {
      result << parseProjectMetadata( it->toObject() );
    }
  }
  else if ( !object.isEmpty() ) // listProjectsbyName API returns projects as separate objects not in array
  {
    for ( auto it = object.begin(); it != object.end(); ++it )
    {
      MerginProject project = parseProjectMetadata( it->toObject() );
      if ( !project.remoteError.isEmpty() )
      {
        // add project namespace/name from object name in case of error
        MerginApi::extractProjectName( it.key(), project.projectNamespace, project.projectName );
      }
      result << project;
    }
  }
  return result;
}


QStringList MerginApi::generateChunkIdsForSize( qint64 fileSize )
{
  qreal rawNoOfChunks = qreal( fileSize ) / UPLOAD_CHUNK_SIZE;
  int noOfChunks = qCeil( rawNoOfChunks );

  // edge case when file is empty, filesize equals zero
  // manually set one chunk so that file will be synced
  if ( fileSize <= 0 )
    noOfChunks = 1;

  QStringList chunks;
  for ( int i = 0; i < noOfChunks; i++ )
  {
    QString chunkID = CoreUtils::uuidWithoutBraces( QUuid::createUuid() );
    chunks.append( chunkID );
  }
  return chunks;
}

QJsonArray MerginApi::prepareUploadChangesJSON( const QList<MerginFile> &files )
{
  QJsonArray jsonArray;

  for ( MerginFile file : files )
  {
    QJsonObject fileObject;
    fileObject.insert( "path", file.path );

    fileObject.insert( "size", file.size );
    fileObject.insert( "mtime", file.mtime.toString( Qt::ISODateWithMs ) );

    if ( !file.diffName.isEmpty() )
    {
      // doing diff-based upload
      QJsonObject diffObject;
      diffObject.insert( "path", file.diffName );
      diffObject.insert( "checksum", file.diffChecksum );
      diffObject.insert( "size", file.diffSize );

      fileObject.insert( "diff", diffObject );
      fileObject.insert( "checksum", file.diffBaseChecksum );
    }
    else
    {
      fileObject.insert( "checksum", file.checksum );
    }

    QJsonArray chunksJson;
    for ( QString id : file.chunks )
    {
      chunksJson.append( id );
    }
    fileObject.insert( "chunks", chunksJson );
    jsonArray.append( fileObject );
  }
  return jsonArray;
}

void MerginApi::finishProjectSync( const QString &projectFullName, bool syncSuccessful )
{
  Q_ASSERT( mTransactionalStatus.contains( projectFullName ) );
  TransactionStatus &transaction = mTransactionalStatus[projectFullName];

  emit syncProjectStatusChanged( projectFullName, -1 );   // -1 means there's no sync going on

  if ( syncSuccessful )
  {
    // update the local metadata file
    writeData( transaction.projectMetadata, transaction.projectDir + "/" + MerginApi::sMetadataFile );

    // update info of local projects
    mLocalProjects.updateLocalVersion( transaction.projectDir, transaction.version );

    CoreUtils::log( "sync " + projectFullName, QStringLiteral( "### Finished ###  New project version: %1\n" ).arg( transaction.version ) );
  }
  else
  {
    CoreUtils::log( "sync " + projectFullName, QStringLiteral( "### FAILED ###\n" ) );
  }

  bool updateBeforeUpload = transaction.updateBeforeUpload;
  QString projectDir = transaction.projectDir;  // keep it before the transaction gets removed
  ProjectDiff diff = transaction.diff;
  int newVersion = syncSuccessful ? transaction.version : -1;
  mTransactionalStatus.remove( projectFullName );

  if ( updateBeforeUpload )
  {
    CoreUtils::log( "sync " + projectFullName, QStringLiteral( "Continue with push after pull" ) );
    // we're done only with the download part before the actual upload - so let's continue with upload
    QString projectNamespace, projectName;
    extractProjectName( projectFullName, projectNamespace, projectName );
    uploadProject( projectNamespace, projectName );
  }
  else
  {
    emit syncProjectFinished( projectDir, projectFullName, syncSuccessful, newVersion );

    if ( syncSuccessful )
    {
      if ( projectFileHasBeenUpdated( diff ) )
      {
        emit reloadProject( projectDir );
      }
      else
      {
        emit projectDataChanged( projectFullName );
      }
    }
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


void MerginApi::createPathIfNotExists( const QString &filePath )
{
  QDir dir;
  if ( !dir.exists( mDataDir ) )
    dir.mkpath( mDataDir );

  QFileInfo newFile( filePath );
  if ( !newFile.absoluteDir().exists() )
  {
    if ( !dir.mkpath( newFile.absolutePath() ) )
    {
      CoreUtils::log( "create path", QString( "Creating a folder failed for path: %1" ).arg( filePath ) );
    }
  }
}

bool MerginApi::isInIgnore( const QFileInfo &info )
{
  return sIgnoreExtensions.contains( info.suffix() ) || sIgnoreFiles.contains( info.fileName() );
}

bool MerginApi::excludeFromSync( const QString &filePath, const MerginConfig &config )
{
  if ( config.isValid && config.selectiveSyncEnabled )
  {
    QFileInfo info( filePath );

    bool isExcludedFormat = sIgnoreImageExtensions.contains( info.suffix().toLower() );

    if ( !isExcludedFormat )
      return false;

    if ( config.selectiveSyncDir.isEmpty() )
    {
      return true; // we are ignoring photos in the entire project
    }
    else if ( filePath.startsWith( config.selectiveSyncDir ) )
    {
      return true; // we are ignoring photo in subfolder
    }
  }
  return false;
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

DownloadQueueItem::DownloadQueueItem( const QString &fp, int s, int v, int rf, int rt, bool diff )
  : filePath( fp ), size( s ), version( v ), rangeFrom( rf ), rangeTo( rt ), downloadDiff( diff )
{
  tempFileName = CoreUtils::uuidWithoutBraces( QUuid::createUuid() );
}
