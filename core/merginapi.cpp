/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "merginapi.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>
#include <QByteArray>
#include <QSet>
#include <QUuid>
#include <QtMath>
#include <QElapsedTimer>
#include <utility>
#include <QDesktopServices>
#ifdef MOBILE_OS
#include <QOAuthUriSchemeReplyHandler>
#else
#include <QOAuthHttpServerReplyHandler>
#endif

#include "projectchecksumcache.h"
#include "coreutils.h"
#include "geodiffutils.h"
#include "localprojectsmanager.h"
#include "../app/enumhelper.h"
#include "merginerrortypes.h"

#include <geodiff.h>
#include <qurlquery.h>

const QString MerginApi::sMetadataFile = QStringLiteral( "/.mergin/mergin.json" );
const QString MerginApi::sMetadataFolder = QStringLiteral( ".mergin" );
const QString MerginApi::sMerginConfigFile = QStringLiteral( "mergin-config.json" );
const QString MerginApi::sDefaultApiRoot = QStringLiteral( "https://app.merginmaps.com" );
const QSet<QString> MerginApi::sIgnoreExtensions = QSet<QString>() << "gpkg-shm" << "gpkg-wal" << "qgs~" << "qgz~" << "pyc" << "swap";
const QSet<QString> MerginApi::sIgnoreImageExtensions = QSet<QString>() << "jpg" << "jpeg" << "png";
const QSet<QString> MerginApi::sIgnoreFiles = QSet<QString>() << "mergin.json" << ".DS_Store";
const int MerginApi::UPLOAD_CHUNK_SIZE = 10 * 1024 * 1024; // Should be the same as on the server
const QString MerginApi::sSyncCanceledMessage = QObject::tr( "Synchronisation canceled" );
#ifdef MOBILE_OS
const QString MerginApi::CALLBACK_URL = QStringLiteral( "https://hello.merginmaps.com/mobile/sso-redirect" );
#else
// We use QHostAddress::Null so that LocalHost is used and if that fails try LocalHostIPv6
// see https://doc.qt.io/qt-6/qoauthhttpserverreplyhandler.html#listen
const QHostAddress MerginApi::OAUTH2_LISTEN_ADDRESS = QHostAddress::Null;
constexpr int MerginApi::OAUTH2_LISTEN_PORT = 10042;
#endif

MerginApi::MerginApi( LocalProjectsManager &localProjects, QObject *parent )
  : QObject( parent )
  , mLocalProjects( localProjects )
  , mDataDir( localProjects.dataDir() )
  , mUserInfo( new MerginUserInfo )
  , mWorkspaceInfo( new MerginWorkspaceInfo )
  , mSubscriptionInfo( new MerginSubscriptionInfo )
  , mUserAuth( new MerginUserAuth )
  , mManager( new QNetworkAccessManager( this ) )
{
  // load cached data if there are any
  const QSettings cache;
  if ( cache.contains( QStringLiteral( "Input/apiRoot" ) ) )
  {
    loadCache();
  }
  else
  {
    // set default api root
    setApiRoot( defaultApiRoot() );
  }

  qRegisterMetaType<Transactions>();

  connect( this, &MerginApi::authChanged, this, &MerginApi::saveAuthData );
  connect( this, &MerginApi::apiRootChanged, this, &MerginApi::pingMergin );
  connect( this, &MerginApi::apiRootChanged, this, &MerginApi::getServerConfig );
  connect( this, &MerginApi::pingMerginFinished, this, &MerginApi::checkMerginVersion );
  connect( this, &MerginApi::workspaceCreated, this, &MerginApi::getUserInfo );
  connect( this, &MerginApi::serverTypeChanged, this, [this]
  {
    if ( mUserAuth->hasAuthData() )
    {
      // do not call /user/profile when user just logged out
      getUserInfo();
    }
  } );
  connect( this, &MerginApi::processInvitationFinished, this, &MerginApi::getUserInfo );
  connect( this, &MerginApi::getWorkspaceInfoFinished, this, &MerginApi::getServiceInfo );
  connect( mUserInfo, &MerginUserInfo::userInfoChanged, this, &MerginApi::userInfoChanged );
  connect( mUserInfo, &MerginUserInfo::activeWorkspaceChanged, this, &MerginApi::activeWorkspaceChanged );
  connect( mUserInfo, &MerginUserInfo::activeWorkspaceChanged, this, &MerginApi::getWorkspaceInfo );
  connect( mUserInfo, &MerginUserInfo::hasWorkspacesChanged, this, &MerginApi::hasWorkspacesChanged );
  connect( mSubscriptionInfo, &MerginSubscriptionInfo::subscriptionInfoChanged, this, &MerginApi::subscriptionInfoChanged );
  connect( mSubscriptionInfo, &MerginSubscriptionInfo::planProductIdChanged, this, &MerginApi::onPlanProductIdChanged );
  connect( mUserAuth, &MerginUserAuth::authChanged, this, &MerginApi::authChanged );
  connect( mUserAuth, &MerginUserAuth::authChanged, this, [this]
  {
    if ( mUserAuth->hasValidToken() )
    {
      // do not call /user/profile when user just logged out
      getUserInfo();
    }
  } );

  //
  // check if the cache is up to date:
  //  - server url and type
  //  - user auth and info
  //  - workspace info
  //

  getServerConfig();
  pingMergin();

  if ( mUserAuth->hasAuthData() )
  {
    connect( this, &MerginApi::pingMerginFinished, this, &MerginApi::getUserInfo, Qt::SingleShotConnection );
    connect( this, &MerginApi::userInfoReplyFinished, this, &MerginApi::getWorkspaceInfo, Qt::SingleShotConnection );
  }
}

void MerginApi::loadCache()
{
  const QSettings settings;
  setApiRoot( settings.value( QStringLiteral( "Input/apiRoot" ) ).toString() );
  int serverType = settings.value( QStringLiteral( "Input/serverType" ) ).toInt();

  mServerType = static_cast<MerginServerType::ServerType>( serverType );

  mUserAuth->loadData();
  mUserInfo->loadData();
}

MerginUserAuth *MerginApi::userAuth() const
{
  return mUserAuth;
}

MerginUserInfo *MerginApi::userInfo() const
{
  return mUserInfo;
}

MerginWorkspaceInfo *MerginApi::workspaceInfo() const
{
  return mWorkspaceInfo;
}

MerginSubscriptionInfo *MerginApi::subscriptionInfo() const
{
  return mSubscriptionInfo;
}

QString MerginApi::listProjects( const QString &searchExpression, const QString &flag, const int page )
{
  const bool authorize = flag != "public";

  if ( ( authorize && !validateAuth() ) || mApiVersionStatus != MerginApiStatus::OK )
  {
    emit listProjectsFailed();
    return {};
  }

  QUrlQuery query;

  if ( flag == "workspace" )
  {
    if ( mUserInfo->activeWorkspaceId() < 0 )
    {
      emit listProjectsFailed();
      return {};
    }

    query.addQueryItem( "only_namespace", mUserInfo->activeWorkspaceName() );
  }
  else if ( flag == "created" )
  {
    query.addQueryItem( "flag", "created" );
  }
  else if ( flag == "shared" )
  {
    query.addQueryItem( "flag", "shared" );
  }
  else if ( flag == "public" )
  {
    query.addQueryItem( "only_public", "true" );
  }

  if ( !searchExpression.isEmpty() )
  {
    query.addQueryItem( "name", searchExpression.toUtf8().toPercentEncoding() );
  }

  query.addQueryItem( "order_params", QStringLiteral( "namespace_asc,name_asc" ) );

  // Required query parameters
  query.addQueryItem( "page", QString::number( page ) );
  query.addQueryItem( "per_page", QString::number( PROJECT_PER_PAGE ) );

  QUrl url( mApiRoot + QStringLiteral( "/v1/project/paginated" ) );
  url.setQuery( query );

  // Even if the authorization is not required, it can be included to fetch more results
  QNetworkRequest request = getDefaultRequest( mUserAuth->hasAuthData() );
  request.setUrl( url );

  QString requestId = CoreUtils::uuidWithoutBraces( QUuid::createUuid() );

  const QNetworkReply *reply = mManager->get( request );
  CoreUtils::log( "list projects", QStringLiteral( "Requesting: " ) + url.toString() );
  connect( reply, &QNetworkReply::finished, this, [this, requestId] {this->listProjectsReplyFinished( requestId );} );

  return requestId;
}

QString MerginApi::listProjectsByName( const QStringList &projectNames )
{
  if ( mApiVersionStatus != MerginApiStatus::OK )
  {
    emit listProjectsFailed();
    return QLatin1String();
  }

  constexpr int maxProjectRequests = 50;
  QStringList projectNamesToRequest( projectNames );

  if ( projectNamesToRequest.count() > maxProjectRequests )
  {
    CoreUtils::log( "list projects by name", QStringLiteral( "Too many local projects: " ) + QString::number( static_cast<int>( projectNames.count() ), 'f', 0 ) );
    const QString msg = tr( "Please remove some projects as the app currently\nonly allows up to %1 downloaded projects." ).arg( maxProjectRequests );
    notifyInfo( msg );
    projectNamesToRequest.erase( projectNamesToRequest.begin() + maxProjectRequests, projectNamesToRequest.end() );
    Q_ASSERT( projectNamesToRequest.count() == maxProjectRequests );
  }

  // Authentification is optional in this case, as there might be public projects without the need to be logged in.
  // We only want to include auth token when user is logged in.
  // User's token, however, might have already expired, so let's just refresh it.
  refreshAuthToken();

  // construct JSON body
  QJsonDocument body;
  QJsonObject projects;
  const QJsonArray projectsArr = QJsonArray::fromStringList( projectNamesToRequest );

  projects.insert( "projects", projectsArr );
  body.setObject( projects );

  const QUrl url( mApiRoot + QStringLiteral( "/v1/project/by_names" ) );

  QNetworkRequest request = getDefaultRequest( true );
  request.setUrl( url );
  request.setRawHeader( "Content-type", "application/json" );

  QString requestId = CoreUtils::uuidWithoutBraces( QUuid::createUuid() );

  const QNetworkReply *reply = mManager->post( request, body.toJson() );
  CoreUtils::log( "list projects by name", QStringLiteral( "Requesting: " ) + url.toString() );
  connect( reply, &QNetworkReply::finished, this, [this, requestId] {this->listProjectsByNameReplyFinished( requestId );} );

  return requestId;
}


void MerginApi::downloadNextItem( const QString &projectId )
{
  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];
  const MerginProjectMetadata project = MerginProjectMetadata::fromJson( transaction.projectMetadata );
  const QString projectFullName = CoreUtils::getFullProjectName( project.projectNamespace, project.name );

  Q_ASSERT( !transaction.downloadQueue.isEmpty() );

  DownloadQueueItem item = transaction.downloadQueue.takeFirst();

  QUrl url( mApiRoot + QStringLiteral( "/v1/project/raw/" ) + projectFullName );
  QUrlQuery query;
  // Handles special chars in a filePath (e.g. prevents to convert "+" sign in to a space)
  query.addQueryItem( "file", item.filePath.toUtf8().toPercentEncoding() );
  query.addQueryItem( "version", QStringLiteral( "v%1" ).arg( item.version ) );
  if ( item.downloadDiff )
    query.addQueryItem( "diff", "true" );
  url.setQuery( query );

  QNetworkRequest request = getDefaultRequest();
  request.setUrl( url );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrTempFileName ), item.tempFileName );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ), projectId );

  QString range;
  if ( item.rangeFrom != -1 && item.rangeTo != -1 )
  {
    range = QStringLiteral( "bytes=%1-%2" ).arg( item.rangeFrom ).arg( item.rangeTo );
    request.setRawHeader( "Range", range.toUtf8() );
  }

  QNetworkReply *reply = mManager->get( request );
  connect( reply, &QNetworkReply::finished, this, [this, item] { downloadItemReplyFinished( item ); } );

  transaction.replyPullItems.insert( reply );

  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Requesting item: " ) + url.toString() +
                  ( !range.isEmpty() ? " Range: " + range : QString() ) );
}

void MerginApi::removeProjectsTempFolder( const QString &projectNamespace, const QString &projectName ) const
{
  if ( projectNamespace.isEmpty() || projectName.isEmpty() )
    return; // otherwise we could remove entire users temp or entire .temp

  const QString path = getTempProjectDir( CoreUtils::getFullProjectName( projectNamespace, projectName ) );
  QDir( path ).removeRecursively();
}

QNetworkRequest MerginApi::getDefaultRequest( const bool withAuth ) const
{
  QNetworkRequest request;
  const QString info = CoreUtils::appInfo();
  request.setRawHeader( "User-Agent", QByteArray( info.toUtf8() ) );
  const QString deviceId = CoreUtils::deviceUuid();
  request.setRawHeader( "X-Device-Id", QByteArray( deviceId.toUtf8() ) );
  if ( withAuth )
  {
    request.setRawHeader( "Authorization", QByteArray( "Bearer " + mUserAuth->authToken() ) );
  }

  return request;
}

bool MerginApi::projectFileHasBeenUpdated( const ProjectDiff &diff )
{
  for ( const QString &filePath : diff.remoteAdded )
  {
    if ( CoreUtils::hasProjectFileExtension( filePath ) )
      return true;
  }

  for ( const QString &filePath : diff.remoteUpdated )
  {
    if ( CoreUtils::hasProjectFileExtension( filePath ) )
      return true;
  }

  return false;
}

bool MerginApi::supportsSelectiveSync() const
{
  return mSupportsSelectiveSync;
}

void MerginApi::setSupportsSelectiveSync( const bool supportsSelectiveSync )
{
  mSupportsSelectiveSync = supportsSelectiveSync;
}

bool MerginApi::apiSupportsSubscriptions() const
{
  return mApiSupportsSubscriptions;
}

void MerginApi::setApiSupportsSubscriptions( const bool apiSupportsSubscriptions )
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

void MerginApi::downloadItemReplyFinished( const DownloadQueueItem &item )
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );
  const QString tempFileName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrTempFileName ) ).toString();
  const QString projectId = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ) ).toString();
  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];
  Q_ASSERT( transaction.replyPullItems.contains( r ) );
  const MerginProjectMetadata project = MerginProjectMetadata::fromJson( transaction.projectMetadata );
  const QString projectFullName = CoreUtils::getFullProjectName( project.projectNamespace, project.name );

  if ( r->error() == QNetworkReply::NoError )
  {
    const QByteArray data = r->readAll();
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Downloaded item (%1 bytes)" ).arg( data.size() ) );
    const QString tempFolder = getTempProjectDir( projectFullName );
    const QString tempFilePath = tempFolder + "/" + tempFileName;
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
    emit syncProjectStatusChanged( projectId, static_cast<qreal>( transaction.transferedSize ) / transaction.totalSize );
    transaction.replyPullItems.remove( r );

    r->deleteLater();

    if ( !transaction.downloadQueue.isEmpty() )
    {
      // one request finished, let's start another one
      downloadNextItem( projectId );
    }
    else if ( transaction.replyPullItems.isEmpty() )
    {
      // nothing else to download and all requests are finished, we're done
      finalizeProjectPull( projectId );
    }
    else
    {
      // no more requests to start, but there are pending requests - let's do nothing and wait
    }
  }
  else if ( transaction.retryCount < TransactionStatus::MAX_RETRY_COUNT && isRetryableNetworkError( r ) )
  {
    transaction.retryCount++;
    transaction.downloadQueue.append( item );

    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Retrying download (attempt %1 of %2)" )
                    .arg( transaction.retryCount ).arg( TransactionStatus::MAX_RETRY_COUNT ) );

    downloadNextItem( projectId );

    emit downloadItemRetried( projectId, transaction.retryCount );
    transaction.replyPullItems.remove( r );
    r->deleteLater();
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    if ( serverMsg.isEmpty() )
    {
      if ( r->error() == QNetworkReply::OperationCanceledError )
        serverMsg = sSyncCanceledMessage;
      else
        serverMsg = r->errorString();
    }
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );
    transaction.replyPullItems.remove( r );
    r->deleteLater();
    if ( !transaction.pullItemsAborting )
    {
      // the first failed request will abort all the other pending requests too, and finish pull with error
      abortPullItems( projectId );
      // signal a networking error - we may retry
      const int httpCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
      emit networkErrorOccurred( serverMsg, httpCode, projectId );
    }
    else
    {
      // do nothing more: we are already aborting requests and handling finalization in abortPullItems()
    }
  }
}

void MerginApi::abortPullItems( const QString &projectId )
{
  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];
  const MerginProjectMetadata project = MerginProjectMetadata::fromJson( transaction.projectMetadata );
  const QString projectFullName = CoreUtils::getFullProjectName( project.projectNamespace, project.name );

  transaction.pullItemsAborting = true;

  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Aborting pending downloads" ) );
  for ( QNetworkReply *r : transaction.replyPullItems )
    r->abort();  // abort will trigger downloadItemReplyFinished slot

  // get rid of the temporary download dir where we may have left some downloaded files
  QDir( getTempProjectDir( projectFullName ) ).removeRecursively();

  if ( transaction.firstTimeDownload )
  {
    Q_ASSERT( !transaction.projectDir.isEmpty() );
    QDir( transaction.projectDir ).removeRecursively();
  }

  finishProjectSync( projectFullName, projectId, false );
}

void MerginApi::cacheServerConfig()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  const QString projectId = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ) ).toString();

  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];
  Q_ASSERT( r == transaction.replyPullServerConfig );

  const MerginProjectMetadata project = MerginProjectMetadata::fromJson( transaction.projectMetadata );
  const QString projectFullName = CoreUtils::getFullProjectName( project.projectNamespace, project.name );

  if ( r->error() == QNetworkReply::NoError )
  {
    const QByteArray data = r->readAll();

    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Downloaded mergin config (%1 bytes)" ).arg( data.size() ) );
    transaction.config = MerginConfig::fromJson( data );

    transaction.replyPullServerConfig->deleteLater();
    transaction.replyPullServerConfig = nullptr;

    prepareDownloadConfig( projectId, true );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    if ( serverMsg.isEmpty() )
    {
      serverMsg = r->errorString();
    }
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Failed to cache mergin config - %1. %2" ).arg( r->errorString(), serverMsg ) );

    transaction.replyPullServerConfig->deleteLater();
    transaction.replyPullServerConfig = nullptr;

    // get rid of the temporary download dir where we may have left some downloaded files
    CoreUtils::removeDir( getTempProjectDir( projectFullName ) );

    if ( transaction.firstTimeDownload )
    {
      CoreUtils::removeDir( transaction.projectDir );
    }

    const int httpCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    emit networkErrorOccurred( serverMsg, httpCode, projectId );

    finishProjectSync( projectFullName, projectId, false );
  }
}


void MerginApi::pushFile( const QString &projectFullName, const QString &projectId, const QString &transactionUUID, const MerginFile &file, const int chunkNo )
{
  if ( !validateAuth() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];

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
  const QUrl url( mApiRoot + QStringLiteral( "/v1/project/push/chunk/%1/%2" ).arg( transactionUUID, chunkID ) );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/octet-stream" );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ), projectFullName );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ), projectId );

  Q_ASSERT( !transaction.replyPushFile );
  transaction.replyPushFile = mManager->post( request, data );
  connect( transaction.replyPushFile, &QNetworkReply::finished, this, &MerginApi::pushFileReplyFinished );

  CoreUtils::log( "push " + projectFullName, QStringLiteral( "Uploading item: " ) + url.toString() );
}

void MerginApi::pushStart( const QString &projectFullName, const QString &projectId, const QByteArray &json )
{
  if ( !validateAuth() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];

  QNetworkRequest request = getDefaultRequest();
  const QUrl url( mApiRoot + QStringLiteral( "/v1/project/push/%1" ).arg( projectFullName ) );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ), projectFullName );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ), projectId );

  Q_ASSERT( !transaction.replyPushStart );
  transaction.replyPushStart = mManager->post( request, json );
  connect( transaction.replyPushStart, &QNetworkReply::finished, this, &MerginApi::pushStartReplyFinished );

  CoreUtils::log( "push " + projectFullName, QStringLiteral( "Starting push request: " ) + url.toString() );
}

void MerginApi::cancelPush( const QString &projectId )
{
  if ( !validateAuth() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  if ( !mTransactionalStatus.contains( projectId ) )
    return;

  const TransactionStatus &transaction = mTransactionalStatus[projectId];
  const QString projectFullName = mLocalProjects.projectFromProjectId( projectId ).fullName();

  CoreUtils::log( "push " + projectFullName, QStringLiteral( "User requested cancel" ) );

  // There is an open transaction, abort it followed by calling cancelUpload again.
  if ( transaction.replyPushProjectInfo )
  {
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Aborting project info request" ) );
    transaction.replyPushProjectInfo->abort();  // will trigger uploadInfoReplyFinished slot and emit sync finished
  }
  else if ( transaction.replyPushStart )
  {
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Aborting upload start" ) );
    transaction.replyPushStart->abort();  // will trigger uploadStartReplyFinished slot and emit sync finished
  }
  else if ( transaction.replyPushFile )
  {
    const QString transactionUUID = transaction.transactionUUID;  // copy transaction uuid as the transaction object will be gone after abort
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Aborting upload file" ) );
    transaction.replyPushFile->abort();  // will trigger pushFileReplyFinished slot and emit sync finished

    // also need to cancel the transaction
    sendPushCancelRequest( projectFullName, projectId, transactionUUID );
  }
  else if ( transaction.replyPushFinish )
  {
    const QString transactionUUID = transaction.transactionUUID;  // copy transaction uuid as the transaction object will be gone after abort
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Aborting upload finish" ) );
    transaction.replyPushFinish->abort();  // will trigger pushFinishReplyFinished slot and emit sync finished

    sendPushCancelRequest( projectFullName, projectId, transactionUUID );
  }
  else
  {
    Q_ASSERT( false );  // unexpected state
  }
}


void MerginApi::sendPushCancelRequest( const QString &projectFullName, const QString &projectId, const QString &transactionUUID )
{
  QNetworkRequest request = getDefaultRequest();
  const QUrl url( mApiRoot + QStringLiteral( "/v1/project/push/cancel/%1" ).arg( transactionUUID ) );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ), projectId );

  const QNetworkReply *reply = mManager->post( request, QByteArray() );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::pushCancelReplyFinished );
  CoreUtils::log( "push " + projectFullName, QStringLiteral( "Requesting upload transaction cancel: " ) + url.toString() );
}

void MerginApi::cancelPull( const QString &projectId )
{
  if ( !mTransactionalStatus.contains( projectId ) )
    return;

  const QString projectFullName = mLocalProjects.projectFromProjectId( projectId ).fullName();
  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "User requested cancel" ) );

  const TransactionStatus &transaction = mTransactionalStatus[projectId];

  if ( transaction.replyPullProjectInfo )
  {
    // we're still fetching project info
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Aborting project info request" ) );
    transaction.replyPullProjectInfo->abort();  // abort will trigger pullInfoReplyFinished() slot
  }
  else if ( transaction.replyPullServerConfig )
  {
    // we're getting server config info
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Aborting server config download" ) );
    transaction.replyPullServerConfig->abort();  // abort will trigger cacheServerConfig slot
  }
  else if ( !transaction.replyPullItems.isEmpty() )
  {
    // we're already downloading some files
    abortPullItems( projectId );
  }
  else
  {
    Q_ASSERT( false );  // unexpected state
  }
}

void MerginApi::pushFinish( const QString &projectFullName, const QString &projectId, const QString &transactionUUID )
{
  if ( !validateAuth() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];

  QNetworkRequest request = getDefaultRequest();
  const QUrl url( mApiRoot + QStringLiteral( "/v1/project/push/finish/%1" ).arg( transactionUUID ) );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ), projectFullName );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ), projectId );

  Q_ASSERT( !transaction.replyPushFinish );
  transaction.replyPushFinish = mManager->post( request, QByteArray() );
  connect( transaction.replyPushFinish, &QNetworkReply::finished, this, &MerginApi::pushFinishReplyFinished );

  CoreUtils::log( "push " + projectFullName, QStringLiteral( "Requesting transaction finish: " ) + transactionUUID );
}

bool MerginApi::pullProject( const QString &projectFullName, const QString &projectId, const bool withAuth )
{
  bool pullHasStarted = false;

  CoreUtils::log( "pull " + projectFullName, "### Starting ###" );

  QNetworkReply *reply = getProjectInfo( projectFullName, projectId, withAuth );
  if ( reply )
  {
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Requesting project info: " ) + reply->request().url().toString() );

    Q_ASSERT( !mTransactionalStatus.contains( projectId ) );
    mTransactionalStatus.insert( projectId, TransactionStatus() );
    mTransactionalStatus[projectId].replyPullProjectInfo = reply;
    mTransactionalStatus[projectId].configAllowed = mSupportsSelectiveSync;
    mTransactionalStatus[projectId].type = TransactionStatus::Pull;

    emit syncProjectStatusChanged( projectId, 0 );

    connect( reply, &QNetworkReply::finished, this, &MerginApi::pullInfoReplyFinished );
    pullHasStarted = true;
  }
  else
  {
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "FAILED to create project info request!" ) );
  }

  return pullHasStarted;
}

bool MerginApi::pushProject( const QString &projectFullName, const QString &projectId, const bool isInitialPush )
{
  bool pushHasStarted = false;

  CoreUtils::log( "push " + projectFullName, "### Starting ###" );

  QNetworkReply *reply = getProjectInfo( projectFullName, projectId );
  if ( reply )
  {
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Requesting project info: " ) + reply->request().url().toString() );

    // create entry about pending upload for the project
    Q_ASSERT( !mTransactionalStatus.contains( projectId ) );
    mTransactionalStatus.insert( projectId, TransactionStatus() );
    mTransactionalStatus[projectId].replyPushProjectInfo = reply;
    mTransactionalStatus[projectId].isInitialPush = isInitialPush;
    mTransactionalStatus[projectId].configAllowed = mSupportsSelectiveSync;
    mTransactionalStatus[projectId].type = TransactionStatus::Push;

    emit syncProjectStatusChanged( projectId, 0 );

    connect( reply, &QNetworkReply::finished, this, &MerginApi::pushInfoReplyFinished );
    pushHasStarted = true;
  }
  else
  {
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "FAILED to create project info request!" ) );
  }

  return pushHasStarted;
}

void MerginApi::authorize( const QString &login, const QString &password )
{
  if ( login.isEmpty() || password.isEmpty() )
  {
    emit authFailed();
    emit notifyError( QStringLiteral( "Please enter your login details" ) );
    return;
  }

  mUserAuth->blockSignals( true );
  mUserAuth->setPassword( password );
  mUserAuth->blockSignals( false );

  QNetworkRequest request = getDefaultRequest( false );
  const QString urlString = mApiRoot + QStringLiteral( "/v1/auth/login" );
  const QUrl url( urlString );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );

  QJsonDocument jsonDoc;
  QJsonObject jsonObject;
  jsonObject.insert( QStringLiteral( "login" ), login );
  jsonObject.insert( QStringLiteral( "password" ), mUserAuth->password() );
  jsonDoc.setObject( jsonObject );
  const QByteArray json = jsonDoc.toJson( QJsonDocument::Compact );

  const QNetworkReply *reply = mManager->post( request, json );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::authorizeFinished );
  CoreUtils::log( "auth", QStringLiteral( "Requesting authorization: " ) + url.toString() );
}

void MerginApi::requestSsoConfig()
{
  if ( !mApiSupportsSso )
  {
    CoreUtils::log( QStringLiteral( "SSO Auth" ), QStringLiteral( "User offline or requested sso auth for server that does not support sso!" ) );
    return;
  }

  QNetworkRequest request = getDefaultRequest( false );
  const QUrl url( mApiRoot + QStringLiteral( "/v2/sso/config" ) );
  request.setUrl( url );

  const QNetworkReply *reply = mManager->get( request );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::ssoConfigReplyFinished );
  CoreUtils::log( "SSO", QStringLiteral( "Requesting sso configuration: " ) + url.toString() );
}

void MerginApi::ssoConfigReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {

    const QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      const QString clientId = doc.object().value( QStringLiteral( "client_id" ) ).toString();
      const QString flowType = doc.object().value( QStringLiteral( "tenant_flow_type" ) ).toString();

      if ( flowType == QLatin1String( "multi" ) )
      {
        // Multi tenant server, ask for email
        CoreUtils::log( "SSO", QStringLiteral( "Configuration retrieved, server is multi tenant" ) );
        emit ssoConfigIsMultiTenant();
      }
      else if ( !clientId.isEmpty() )
      {
        // Single tenant, proceed with oauth2 flow
        CoreUtils::log( "SSO", QStringLiteral( "Configuration retrieved, server is single tenant" ) );
        startSsoFlow( clientId );
      }
      else
      {
        CoreUtils::log( "SSO", QStringLiteral( "Server is single tenant but did not return a clientId, not good!" ) );
        emit notifyError( tr( "There is a problem with the server's SSO configuration. Contact the administrator." ) );
      }
    }
  }
  else
  {
    const QString serverMsg = extractServerErrorMsg( r->readAll() );
    CoreUtils::log( "SSO", QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );
    emit notifyError( tr( "Error getting the SSO configuration from the server" ) );
  }

  r->deleteLater();
}

void MerginApi::requestSsoConnections( const QString &email )
{
  if ( !mApiSupportsSso )
  {
    CoreUtils::log( QStringLiteral( "SSO Auth" ), QStringLiteral( "User offline or requested sso auth for server that does not support sso!" ) );
    return;
  }

  QNetworkRequest request = getDefaultRequest( false );
  QUrl url( mApiRoot + QStringLiteral( "/v2/sso/connections" ) );
  QUrlQuery query;
  query.addQueryItem( QStringLiteral( "email" ), email.toUtf8().toPercentEncoding() );
  url.setQuery( query );
  request.setUrl( url );

  const QNetworkReply *reply = mManager->get( request );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::ssoConnectionsReplyFinished );
  CoreUtils::log( "SSO", QStringLiteral( "Requesting available connections: " ) + url.toString() );
}

void MerginApi::ssoConnectionsReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {

    const QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      const QString clientId = doc.object().value( QStringLiteral( "id" ) ).toString();

      if ( clientId.isEmpty() )
      {
        // no connection found for requested domain
        // should not happen, as the server would return 404 in that case
        CoreUtils::log( "SSO", QStringLiteral( "No Connections available for the specified domain" ) );
        emit notifyError( tr( "SSO is not supported for the specified domain" ) );
      }
      else
      {
        startSsoFlow( clientId );
      }
    }
  }
  else
  {
    const QString serverMsg = extractServerErrorMsg( r->readAll() );
    CoreUtils::log( "SSO", QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );
    const int statusCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    if ( statusCode == 404 )
      emit notifyError( tr( "SSO is not supported for the specified domain" ) );
    else
      emit notifyError( tr( "Error getting the SSO configuration from the server" ) );
  }

  r->deleteLater();
}

void MerginApi::registerUser( const QString &email,
                              const QString &password,
                              const bool acceptedTOC )
{
  // Some very basic checks, so we do not validate everything
  if ( !CoreUtils::isValidEmail( email ) )
  {
    const QString msg = tr( "Please enter a valid email" );
    emit registrationFailed( msg, RegistrationError::RegistrationErrorType::EMAIL );
    return;
  }

  if ( password.isEmpty() || password.length() < 8 )
  {
    const QString msg = tr( "%1%3 Password must be at least 8 characters long and include:"
                            "<ul type=\"disc\">"
                            "%3 Lowercase characters (a-z)%4"
                            "%3 Uppercase characters (A-Z)%4"
                            "%3 At least one digit (0–9) or special character%4"
                            "%2%4%2" )
                        .arg( "<ul>", "</ul>", "<li>", "</li>" );
    emit registrationFailed( msg, RegistrationError::RegistrationErrorType::PASSWORD );
    return;

  }

  if ( !acceptedTOC )
  {
    const QString msg = tr( "Please accept Terms and Privacy Policy" );
    emit registrationFailed( msg, RegistrationError::RegistrationErrorType::TOC );
    return;
  }

  // request
  QNetworkRequest request = getDefaultRequest( false );
  const QString urlString = mApiRoot + QStringLiteral( "/v1/auth/register" );
  const QUrl url( urlString );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );

  QJsonDocument jsonDoc;
  QJsonObject jsonObject;
  jsonObject.insert( QStringLiteral( "email" ), email );
  jsonObject.insert( QStringLiteral( "password" ), password );
  jsonObject.insert( QStringLiteral( "api_key" ), getApiKey( mApiRoot ) );
  jsonDoc.setObject( jsonObject );
  const QByteArray json = jsonDoc.toJson( QJsonDocument::Compact );
  const QNetworkReply *reply = mManager->post( request, json );
  connect( reply, &QNetworkReply::finished, this, [ = ] { this->registrationFinished( email, password ); } );
  CoreUtils::log( "auth", QStringLiteral( "Requesting registration: " ) + url.toString() );
}

void MerginApi::postRegisterUser( const QString &marketingChannel, const QString &industry, const bool wantsNewsletter )
{
  // Some very basic checks, so we do not validate everything
  if ( marketingChannel.isEmpty() || industry.isEmpty() )
  {
    const QString msg = tr( "Marketing source cannot be empty" );
    emit postRegistrationFailed( msg );
    return;
  }

  // Some very basic checks, so we do not validate everything
  if ( industry.isEmpty() )
  {
    const QString msg = tr( "Industry cannot be empty" );
    emit postRegistrationFailed( msg );
    return;
  }
  // request
  QNetworkRequest request = getDefaultRequest( false );
  const QString urlString = mApiRoot + QStringLiteral( "/v1/post-register" );
  const QUrl url( urlString );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );

  QJsonDocument jsonDoc;
  QJsonObject jsonObject;
  jsonObject.insert( QStringLiteral( "industry" ), industry );
  jsonObject.insert( QStringLiteral( "marketing_channel" ), marketingChannel );
  jsonObject.insert( QStringLiteral( "subscribe" ), wantsNewsletter );
  jsonDoc.setObject( jsonObject );
  const QByteArray json = jsonDoc.toJson( QJsonDocument::Compact );
  const QNetworkReply *reply = mManager->post( request, json );
  connect( reply, &QNetworkReply::finished, this, [ = ] { this->postRegistrationFinished(); } );
  CoreUtils::log( "auth", QStringLiteral( "Requesting post-registration: " ) + url.toString() );
}

void MerginApi::getUserInfo()
{
  if ( !validateAuth() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QString urlString;
  if ( mServerType == MerginServerType::OLD )
  {
    if ( mUserAuth->authMethod() == MerginUserAuth::SSO )
    {
      CoreUtils::log( QStringLiteral( "Server API" ), QStringLiteral( "SSO user info is not available on old servers" ) );
      return;
    }
    urlString = mApiRoot + QStringLiteral( "/v1/user/%1" ).arg( mUserAuth->login() );
  }
  else
  {
    urlString = mApiRoot + QStringLiteral( "/v1/user/profile" );
  }

  QNetworkRequest request = getDefaultRequest( true );
  const QUrl url( urlString );
  request.setUrl( url );

  const QNetworkReply *reply = mManager->get( request );
  CoreUtils::log( "user info", QStringLiteral( "Requesting user info: " ) + url.toString() );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::getUserInfoFinished );
}

void MerginApi::getWorkspaceInfo()
{
  if ( mServerType == MerginServerType::OLD )
  {
    return;
  }

  if ( mUserInfo->activeWorkspaceId() == -1 )
  {
    return;
  }

  if ( !validateAuth() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  const QString urlString = mApiRoot + QStringLiteral( "/v1/workspace/%1" ).arg( mUserInfo->activeWorkspaceId() );
  QNetworkRequest request = getDefaultRequest();
  const QUrl url( urlString );
  request.setUrl( url );

  const QNetworkReply *reply = mManager->get( request );
  CoreUtils::log( "workspace info", QStringLiteral( "Requesting workspace info: " ) + url.toString() );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::getWorkspaceInfoReplyFinished );
}

void MerginApi::getServiceInfo()
{
  if ( !validateAuth() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QString urlString;

  if ( mServerType == MerginServerType::SAAS )
  {
    if ( mUserInfo->activeWorkspaceId() < 0 )
    {
      CoreUtils::log( QStringLiteral( "Service Info" ), QStringLiteral( "Skipped calling GET service info, no active workspace detected" ) );
      return;
    }

    urlString = mApiRoot + QStringLiteral( "/v1/workspace/%1/service" ).arg( mUserInfo->activeWorkspaceId() );
  }
  else if ( mServerType == MerginServerType::OLD )
  {
    urlString = mApiRoot + QStringLiteral( "/v1/user/service" );
  }
  else
  {
    return;
  }

  QNetworkRequest request = getDefaultRequest( true );
  const QUrl url( urlString );
  request.setUrl( url );

  const QNetworkReply *reply = mManager->get( request );

  connect( reply, &QNetworkReply::finished, this, &MerginApi::getServiceInfoReplyFinished );

  CoreUtils::log( "Service info", QStringLiteral( "Requesting service info: " ) + url.toString() );
}

void MerginApi::getServiceInfoReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "Service info", QStringLiteral( "Success" ) );

    const QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      const QJsonObject docObj = doc.object();
      mSubscriptionInfo->setFromJson( docObj );
    }
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    const QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "getServiceInfo" ), r->errorString(), serverMsg );
    CoreUtils::log( "Service info", QStringLiteral( "FAILED - %1" ).arg( message ) );

    mSubscriptionInfo->clear();

    const int httpCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    if ( httpCode == 404 )
    {
      // no such API on the server, do not emit anything
    }
    else if ( httpCode == 403 )
    {
      // forbidden - I do not have enough rights to see this, do not emit anything
    }
    else
    {
      emit networkErrorOccurred( serverMsg );
    }
  }

  r->deleteLater();
}

void MerginApi::clearAuth()
{
  mUserAuth->blockSignals( true );
  mUserInfo->blockSignals( true );
  mWorkspaceInfo->blockSignals( true );
  mSubscriptionInfo->blockSignals( true );

  mUserAuth->clear();
  mUserInfo->clear();
  mWorkspaceInfo->clear();
  mSubscriptionInfo->clear();

  mUserAuth->blockSignals( false );
  mUserInfo->blockSignals( false );
  mWorkspaceInfo->blockSignals( false );
  mSubscriptionInfo->blockSignals( false );

  emit subscriptionInfoChanged();
  emit workspaceInfoChanged();
  emit mUserInfo->activeWorkspaceChanged();
  emit mUserInfo->hasWorkspacesChanged();
  emit mUserInfo->userInfoChanged();
  emit authChanged();

  CoreUtils::log( QStringLiteral( "Auth" ), QStringLiteral( "Cleared auth and user data cache" ) );
}

QString MerginApi::resetPasswordUrl() const
{
  if ( !mApiRoot.isEmpty() )
  {
    const QUrl base( mApiRoot );
    return base.resolved( QUrl( "login/reset" ) ).toString();
  }
  return {};
}

bool MerginApi::createProject( const QString &projectNamespace, const QString &projectName, const QString &projectId, const bool isPublic )
{
  if ( !validateAuth() )
  {
    emit missingAuthorizationError( projectId );
    return false;
  }

  if ( mApiVersionStatus != MerginApiStatus::OK )
  {
    return false;
  }

  QNetworkRequest request = getDefaultRequest();
  const QUrl url( mApiRoot + QString( "/v1/project/%1" ).arg( projectNamespace ) );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );
  request.setRawHeader( "Accept", "application/json" );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ), projectId );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ),
                        CoreUtils::getFullProjectName( projectNamespace, projectName ) );

  QJsonDocument jsonDoc;
  QJsonObject jsonObject;
  jsonObject.insert( QStringLiteral( "name" ), projectName );
  jsonObject.insert( QStringLiteral( "public" ), isPublic );
  jsonDoc.setObject( jsonObject );
  const QByteArray json = jsonDoc.toJson( QJsonDocument::Compact );

  const QNetworkReply *reply = mManager->post( request, json );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::createProjectFinished );
  CoreUtils::log( "create " + CoreUtils::getFullProjectName( projectNamespace, projectName ),
                  QStringLiteral( "Requesting project creation: " ) + url.toString() );

  return true;
}

void MerginApi::deleteProject( const QString &projectId, bool informUser )
{
  if ( !validateAuth() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QNetworkRequest request = getDefaultRequest();
  const QUrl url( mApiRoot + QStringLiteral( "/v2/projects/%1" ).arg( projectId ) );
  request.setUrl( url );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ), projectId );
  const QNetworkReply *reply = mManager->deleteResource( request );
  connect( reply, &QNetworkReply::finished, this, [this, informUser] { this->deleteProjectFinished( informUser );} );
  CoreUtils::log( "delete " + projectId, QStringLiteral( "Requesting immediate project deletion: " ) + url.toString() );
}

void MerginApi::saveAuthData() const
{
  QSettings settings;
  settings.beginGroup( "Input/" );
  settings.setValue( "apiRoot", mApiRoot );
  settings.endGroup();

  mUserAuth->saveData();
}

void MerginApi::createProjectFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  const QString projectId = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ) ).toString();
  const QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "create " + projectFullName, QStringLiteral( "Success" ) );

    // Upload data if createProject has been called for a local project with empty namespace (case of migrating a project)
    if ( mLocalProjects.projects().contains( projectId ) )
    {
      // we shoot this signal for tests
      emit projectCreated( projectId, true );
      // we remove the process saved under the old ID and pushProject will insert new process with new ID
      emit projectCreated( projectId, false );

      const QNetworkReply *reply = getProjectInfo( projectFullName, projectId );
      connect( reply, &QNetworkReply::finished, this, [ this, projectId, projectFullName ]
      {
        QNetworkReply *reply = qobject_cast<QNetworkReply *>( sender() );
        Q_ASSERT( reply );

        if ( reply->error() == QNetworkReply::NoError )
        {
          const QByteArray data = reply->readAll();
          const MerginProjectMetadata serverProject = MerginProjectMetadata::fromJson( data );

          mLocalProjects.updateProjectId( projectId, serverProject.projectId );
          mLocalProjects.updateNamespace( serverProject.projectId, serverProject.projectNamespace );
          emit projectAttachedToMergin( serverProject.projectId );

          const LocalProject info = mLocalProjects.projectFromProjectId( serverProject.projectId );
          const QDir projectDir( info.projectDir );
          if ( projectDir.exists() && !projectDir.isEmpty() )
          {
            pushProject( projectFullName, serverProject.projectId, true );
          }
        }
        else
        {
          CoreUtils::log( "create " + projectFullName, QString( "Failed to get new ID for project %1" ).arg( projectFullName ) );
        }

        reply->deleteLater();
      } );
    }
    else
    {
      emit projectCreated( projectId, true );
    }
  }
  else
  {
    const QByteArray data = r->readAll();
    const QString code = extractServerErrorCode( data );
    QString serverMsg = extractServerErrorMsg( data );
    const QString message = QStringLiteral( "FAILED - %1: %2" ).arg( r->errorString(), serverMsg );
    const int httpCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    const bool showLimitReachedDialog = EnumHelper::isEqual( code, ErrorCode::ProjectsLimitHit );
    const bool userMissingPermissions = ( httpCode == 403 );

    CoreUtils::log( "create " + projectFullName, message );

    emit projectCreated( projectId, false );

    if ( showLimitReachedDialog )
    {
      int maxProjects = 0;
      const QVariant maxProjectVariant = extractServerErrorValue( data, "projects_quota" );
      if ( maxProjectVariant.isValid() )
        maxProjects = maxProjectVariant.toInt();
      emit projectLimitReached( maxProjects, serverMsg );
    }
    else if ( userMissingPermissions )
    {
      emit notifyError( tr( "You don't have permission to create new projects in this workspace." ) );
      emit projectCreationFailed();
    }
    else
    {
      emit notifyError( tr( "Couldn't create the project. Please try again later or contact support if the problem persists." ) );
      emit projectCreationFailed();
      emit networkErrorOccurred( serverMsg, QStringLiteral( "Mergin API error: createProject" ), httpCode, projectId );
    }
  }
  r->deleteLater();
}

void MerginApi::deleteProjectFinished( const bool informUser )
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  const QString projectId = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ) ).toString();

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "delete " + projectId, QStringLiteral( "Success" ) );

    if ( informUser )
      emit notifySuccess( QStringLiteral( "Project deleted" ) );

  }
  else
  {
    const QString serverMsg = extractServerErrorMsg( r->readAll() );
    const int serverErrorCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    CoreUtils::log( "delete " + projectId, QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );
    emit networkErrorOccurred( serverMsg, serverErrorCode );
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
    const QJsonDocument doc = QJsonDocument::fromJson( data );
    if ( doc.isObject() )
    {
      const QJsonObject docObj = doc.object();
      mUserAuth->setFromJson( docObj );
    }
    else
    {
      // keep login and password, but clear token
      // this is problem with internet connection or server
      // so do not force user to input login credentials again
      mUserAuth->clearTokenData();
      emit authFailed();
      CoreUtils::log( "Auth", QStringLiteral( "FAILED - invalid JSON response" ) );
      emit notifyError( "Internal server error during authorization" );
    }
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    const QVariant statusCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    const int status = statusCode.toInt();
    CoreUtils::log( "Auth", QStringLiteral( "FAILED - %1. %2 (%3)" ).arg( r->errorString(), serverMsg, QString::number( status ) ) );

    if ( status == 401 )
    {
      // OK, we have INVALID login or password or
      // our user got blocked on the server by admin or owner
      // lets show error to user and let him try different credentials
      emit authFailed();
      emit notifyError( serverMsg );

      mUserAuth->blockSignals( true );
      mUserAuth->setLogin( QString() );
      mUserAuth->setPassword( QString() );
      mUserAuth->blockSignals( false );

    }
    else
    {
      // keep login and password
      // this is problem with internet connection or server
      // so do not force user to input login credentials again
      emit networkErrorOccurred( serverMsg );
    }

    // in case of any error, just clean token and request new one
    mUserAuth->clearTokenData();
  }

  if ( mAuthLoopEvent.isRunning() )
  {
    mAuthLoopEvent.exit();
  }
  r->deleteLater();
}

void MerginApi::registrationFinished( const QString &login, const QString &password )
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "register", QStringLiteral( "Success" ) );
    const QString msg = tr( "Registration successful" );
    emit notifySuccess( msg );

    if ( !login.isEmpty() && !password.isEmpty() ) // log in immediately
      authorize( login, password );

    emit registrationSucceeded();
  }
  else
  {
    const QByteArray data = r->readAll();
    const QString serverErrorCode = extractServerErrorCode( data );
    QString serverMsg = extractServerErrorMsg( data );
    CoreUtils::log( "register", QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );
    const QVariant statusCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    const int status = statusCode.toInt();
    if ( status == 401 || status == 400 )
    {
      if ( serverErrorCode == "InvalidUsername" || serverErrorCode == "InvalidEmail" )
      {
        const QString msg = tr( "Please enter a valid email" );
        emit registrationFailed( msg, RegistrationError::RegistrationErrorType::EMAIL );
        emit notifyError( tr( "Registration failed" ) );
      }
      else if ( serverErrorCode == "ExistingEmail" )
      {
        const QString msg = tr( "This email address is already registered" );
        emit registrationFailed( msg, RegistrationError::RegistrationErrorType::EMAIL );
        emit notifyError( tr( "Registration failed" ) );
      }
      else if ( serverErrorCode == "InvalidPassword" )
      {
        const QString msg = tr( "%1%3 Password must be at least 8 characters long and include:"
                                "<ul type=\"disc\">"
                                "%3 Lowercase characters (a-z)%4"
                                "%3 Uppercase characters (A-Z)%4"
                                "%3 At least one digit (0–9) or special character%4"
                                "%2%4%2" )
                            .arg( "<ul>", "</ul>", "<li>", "</li>" );
        emit registrationFailed( msg, RegistrationError::RegistrationErrorType::PASSWORD );
        emit notifyError( tr( "Registration failed" ) );
      }
      else
      {
        emit registrationFailed( serverMsg, RegistrationError::RegistrationErrorType::OTHER );
        emit notifyError( serverMsg );
      }
    }
    else if ( status == 404 )
    {
      // the self-registration is not allowed on the server
      const QString msg = tr( "New registrations are not allowed on the selected server. Please check with your administrator." );
      emit registrationFailed( msg, RegistrationError::RegistrationErrorType::OTHER );
      emit notifyError( msg );
    }
    else
    {
      const QString msg = QStringLiteral( "Mergin API error: register" );
      emit registrationFailed( msg, RegistrationError::RegistrationErrorType::OTHER );
      emit networkErrorOccurred( serverMsg );
    }
  }
  r->deleteLater();
}

void MerginApi::postRegistrationFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "post-register", QStringLiteral( "Success" ) );
    emit postRegistrationSucceeded();
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    CoreUtils::log( "post-register", QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );
    emit postRegistrationFailed( QStringLiteral( "Post-registration failed %1" ).arg( serverMsg ) );
  }
  emit notifySuccess( tr( "Workspace created" ) );
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
    const QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      const QJsonObject obj = doc.object();
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
    if ( mServerType == MerginServerType::OLD )
    {
      getUserInfo();
    }
    else
    {
      getWorkspaceInfo();
    }
  }
}

QNetworkReply *MerginApi::getProjectInfo( const QString &projectFullName, const QString &projectId, const bool withAuth )
{
  if ( withAuth && !validateAuth() )
  {
    emit missingAuthorizationError( projectId );
    return nullptr;
  }

  if ( mApiVersionStatus != MerginApiStatus::OK )
  {
    return nullptr;
  }

  int sinceVersion = -1;
  const LocalProject projectInfo = mLocalProjects.projectFromProjectId( projectId );
  if ( projectInfo.isValid() )
  {
    // let's also fetch the recent history of diffable files
    // (the "since" is inclusive, so if we are on v2, we want to use since=v3 which will include v2->v3, v3->v4, ...)
    sinceVersion = projectInfo.localVersion + 1;
  }

  QUrlQuery query;
  if ( sinceVersion != -1 )
    query.addQueryItem( QStringLiteral( "since" ), QStringLiteral( "v%1" ).arg( sinceVersion ) );

  QUrl url{};
  url.setUrl( mApiRoot + QStringLiteral( "/v1/project/%1" ).arg( projectFullName ) );
  url.setQuery( query );

  QNetworkRequest request = getDefaultRequest( withAuth );
  request.setUrl( url );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ), projectId );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ), projectFullName );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrAuthUsed ), withAuth );

  return mManager->get( request );
}

QNetworkReply *MerginApi::getProjectDetails( const QString &projectId, const bool withAuth )
{
  if ( withAuth && !validateAuth() )
  {
    emit missingAuthorizationError( projectId );
    return nullptr;
  }

  if ( mApiVersionStatus != MerginApiStatus::OK )
  {
    return nullptr;
  }

  QUrl url{};
  url.setUrl( mApiRoot + QStringLiteral( "/v1/project/by_uuid/%1" ).arg( projectId ) );

  QNetworkRequest request = getDefaultRequest( withAuth );
  request.setUrl( url );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ), projectId );

  return mManager->get( request );
}

QNetworkReply *MerginApi::getProjectsDetails( const QStringList &projectIds, const bool withAuth )
{
  if ( withAuth && !validateAuth() )
  {
    for ( const QString& projectId : projectIds )
    {
      emit missingAuthorizationError( projectId );
    }
    return nullptr;
  }

  if ( mApiVersionStatus != MerginApiStatus::OK )
  {
    return nullptr;
  }

  QUrlQuery query;
  query.addQueryItem( QStringLiteral( "uuids" ), projectIds.join(",") );

  QUrl url{};
  url.setUrl( mApiRoot + QStringLiteral( "/v1/project/by_uuids" ) );
  url.setQuery( query );

  QNetworkRequest request = getDefaultRequest( withAuth );
  request.setUrl( url );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrAuthUsed ), withAuth );

  return mManager->get( request );
}

bool MerginApi::validateAuth()
{
  if ( !mUserAuth->hasAuthData() )
  {
    emit authRequested();
    return false;
  }

  if ( mUserAuth->authToken().isEmpty() || mUserAuth->tokenExpiration() < QDateTime::currentDateTimeUtc() )
  {
    CoreUtils::log( QStringLiteral( "MerginApi" ), QStringLiteral( "Requesting authorization because of missing or expired token." ) );

    switch ( mUserAuth->authMethod() )
    {
      case MerginUserAuth::AuthMethod::SSO:
        // we need to request auth again
        emit ssoLoginExpired();
        return false;
      case MerginUserAuth::AuthMethod::Password:
        refreshAuthToken();
        return true;
    }
  }

  return true;
}

void MerginApi::checkMerginVersion( const QString &apiVersion, const bool serverSupportsSubscriptions, const QString &msg )
{
  setApiSupportsSubscriptions( serverSupportsSubscriptions );

  if ( msg.isEmpty() )
  {
    int major = -1;
    int minor = -1;

    const bool validVersion = parseVersion( apiVersion, major, minor );

    if ( !validVersion )
    {
      setApiVersionStatus( MerginApiStatus::NOT_FOUND );
      return;
    }

    if ( ( MERGIN_API_VERSION_MAJOR == major && MERGIN_API_VERSION_MINOR <= minor ) || MERGIN_API_VERSION_MAJOR < major )
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

QString MerginApi::extractServerErrorCode( const QByteArray &data )
{
  const QVariant code = extractServerErrorValue( data, QStringLiteral( "code" ) );
  if ( code.isValid() )
    return code.toString();
  return {};
}

QVariant MerginApi::extractServerErrorValue( const QByteArray &data, const QString &key )
{
  const QJsonDocument doc = QJsonDocument::fromJson( data );
  if ( doc.isObject() )
  {
    const QJsonObject obj = doc.object();
    if ( obj.contains( key ) )
    {
      const QJsonValue val = obj.value( key );
      return val.toVariant();
    }
  }

  return {};
}

QString MerginApi::extractServerErrorMsg( const QByteArray &data )
{
  QString serverMsg = "[can't parse server error]";
  const QJsonDocument doc = QJsonDocument::fromJson( data );
  if ( doc.isObject() )
  {
    const QJsonObject obj = doc.object();
    if ( obj.contains( QStringLiteral( "detail" ) ) )
    {
      const QJsonValue vDetail = obj.value( "detail" );
      if ( vDetail.isString() )
      {
        serverMsg = vDetail.toString();
      }
      else if ( vDetail.isObject() )
      {
        serverMsg = QJsonDocument( vDetail.toObject() ).toJson();
      }
    }
    else if ( obj.contains( QStringLiteral( "name" ) ) )
    {
      const QJsonValue val = obj.value( "name" );
      if ( val.isArray() )
      {
        const QJsonArray errors = val.toArray();
        QStringList messages;
        for ( auto it = errors.constBegin(); it != errors.constEnd(); ++it )
        {
          messages << it->toString();
        }
        serverMsg = messages.join( " " );
      }
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


LocalProject MerginApi::getLocalProject( const QString &projectId ) const
{
  return mLocalProjects.projectFromProjectId( projectId );
}

ProjectDiff MerginApi::localProjectChanges( const QString &projectDir )
{
  const MerginProjectMetadata projectMetadata = MerginProjectMetadata::fromCachedJson( projectDir + "/" + sMetadataFile );
  const QList<MerginFile> localFiles = getLocalProjectFiles( projectDir + "/" );

  const MerginConfig config = MerginConfig::fromFile( projectDir + "/" + sMerginConfigFile );

  return compareProjectFiles( projectMetadata.files, projectMetadata.files, localFiles, projectDir, config.isValid, config );
}

bool MerginApi::parseVersion( const QString &version, int &major, int &minor )
{
  if ( version.isNull() || version.isEmpty() )
    return false;

  QStringList versionParts = version.split( '.' );

  if ( versionParts.size() != 3 )
    return false;

  bool majorOk, minorOk;
  major = versionParts[0].toInt( &majorOk );
  minor = versionParts[1].toInt( &minorOk );

  if ( !majorOk || !minorOk )
    return false;

  return true;
}

bool MerginApi::hasLocalProjectChanges( const QString &projectDir, const bool supportsSelectiveSync )
{
  const MerginProjectMetadata projectMetadata = MerginProjectMetadata::fromCachedJson( projectDir + "/" + sMetadataFile );
  const QList<MerginFile> localFiles = getLocalProjectFiles( projectDir + "/" );

  MerginConfig config;
  if ( supportsSelectiveSync )
  {
    config = MerginConfig::fromFile( projectDir + "/" + sMerginConfigFile );
  }

  return hasLocalChanges( projectMetadata.files, localFiles, projectDir, config );
}

QString MerginApi::getTempProjectDir( const QString &projectFullName ) const
{
  return mDataDir + "/" + TEMP_FOLDER + projectFullName;
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
  const QUrl url( mApiRoot + QStringLiteral( "/ping" ) );
  request.setUrl( url );

  const QNetworkReply *reply = mManager->get( request );
  CoreUtils::log( "ping", QStringLiteral( "Requesting: " ) + url.toString() );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::pingMerginReplyFinished );
}

void MerginApi::detachProjectFromMergin( const QString &projectId, const bool informUser )
{
  // Remove mergin folder
  const LocalProject projectInfo = mLocalProjects.projectFromProjectId( projectId );

  if ( projectInfo.isValid() )
  {
    CoreUtils::removeDir( projectInfo.projectDir + "/.mergin" );
  }

  // Update localProject
  mLocalProjects.updateNamespace( projectInfo.projectDir, "" );
  mLocalProjects.updateLocalVersion( projectInfo.projectDir, -1 );

  if ( informUser )
    emit notifySuccess( tr( "Project detached from the server" ) );

  emit projectDetached( projectId );
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

  // Api root should not include the trailing slash!
  while ( newApiRoot.endsWith( "/" ) )
  {
    newApiRoot.chop( 1 );
  }

  if ( newApiRoot != mApiRoot )
  {
    mApiRoot = newApiRoot;

    QSettings settings;
    settings.setValue( QStringLiteral( "Input/apiRoot" ), mApiRoot );

    emit apiRootChanged();
  }
}

QList<MerginFile> MerginApi::getLocalProjectFiles( const QString &projectPath )
{
  QElapsedTimer timer;
  timer.start();

  QList<MerginFile> merginFiles;
  ProjectChecksumCache checksumCache( projectPath );

  QSet<QString> localFiles = listFiles( projectPath );
  for ( const QString &p : localFiles )
  {
    MerginFile file;
    file.checksum = checksumCache.get( p );
    file.path = p;
    QFileInfo info( projectPath + p );
    file.size = info.size();
    file.mtime = info.lastModified();
    merginFiles.append( file );
  }

  const qint64 elapsed = timer.elapsed();
  if ( elapsed > 100 )
  {
    CoreUtils::log( "Local File", QStringLiteral( "It took %1 ms to create MerginFiles for %2 local files for %3." ).arg( elapsed ).arg( localFiles.count() ).arg( projectPath ) );
  }
  return merginFiles;
}

void MerginApi::listProjectsReplyFinished( const QString &requestId )
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  int projectCount = -1;
  int requestedPage = 1;
  MerginProjectsList projectList;

  if ( r->error() == QNetworkReply::NoError )
  {
    const QUrlQuery query( r->request().url().query() );
    requestedPage = query.queryItemValue( "page" ).toInt();

    const QByteArray data = r->readAll();
    const QJsonDocument doc = QJsonDocument::fromJson( data );

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
    const QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "listProjects" ), r->errorString(), serverMsg );
    emit networkErrorOccurred( serverMsg );
    CoreUtils::log( "list projects", QStringLiteral( "FAILED - %1" ).arg( message ) );

    emit listProjectsFailed();
  }

  r->deleteLater();

  emit listProjectsFinished( projectList, projectCount, requestedPage, requestId );
}

void MerginApi::listProjectsByNameReplyFinished( const QString &requestId )
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  MerginProjectsList projectList;

  if ( r->error() == QNetworkReply::NoError )
  {
    const QByteArray data = r->readAll();
    const QJsonDocument json = QJsonDocument::fromJson( data );
    projectList = parseProjectsFromJson( json );
    CoreUtils::log( "list projects by name", QStringLiteral( "Success - got %1 projects" ).arg( projectList.count() ) );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    const QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "listProjectsByName" ), r->errorString(), serverMsg );
    emit networkErrorOccurred( serverMsg );
    CoreUtils::log( "list projects by name", QStringLiteral( "FAILED - %1" ).arg( message ) );

    emit listProjectsFailed();
  }

  r->deleteLater();

  emit listProjectsByNameFinished( projectList, requestId );
}

void MerginApi::getProjectsDetailsReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    const QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      const QJsonObject response = doc.object();
      for ( const QString key : response.keys() )
      {
        // if the selected project ID is in pending transactions we need to restart the whole process as this call
        // was a fallback
        if ( mTransactionalStatus.contains( key ) )
        {
          QJsonObject project = response.value( key ).toObject();
          QString projectFullName = QString("%1/%2").arg( project.value("namespace").toString(), project.value("name").toString());
          const bool withAuth = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrAuthUsed ) ).toBool();
          if (mTransactionalStatus[key].type == TransactionStatus::Pull )
          {
            pullProject( projectFullName, key, withAuth);
          } else
          {
            pushProject( projectFullName, key, withAuth);
          }
        }
      }
    }
  }
}


void MerginApi::finalizeProjectPullCopy( const QString &projectFullName, const QString &projectDir, const QString &tempDir, const QString &filePath, const QList<DownloadQueueItem> &items )
{
  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Copying new content of " ) + filePath );

  const QString dest = projectDir + "/" + filePath;
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

  // if diffable, copy to .mergin dir so we have a base file
  if ( isFileDiffable( filePath ) )
  {
    const QString baseFile = projectDir + "/.mergin/" + filePath;
    createPathIfNotExists( baseFile );

    if ( !QFile::remove( baseFile ) )
    {
      CoreUtils::log( "pull " + projectFullName, "failed to remove old base file for: " + filePath );
    }
    if ( !QFile::copy( dest, baseFile ) )
    {
      CoreUtils::log( "pull " + projectFullName, "failed to copy new base file for: " + filePath );
    }
  }
}


bool MerginApi::finalizeProjectPullApplyDiff( const QString &projectFullName, const QString &projectId, const QString &projectDir, const QString &tempDir, const QString &filePath, const QList<DownloadQueueItem> &items )
{
  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Applying diff to " ) + filePath );

  // update diffable files that have been modified on the server
  // - if they were not modified locally, the server changes will be simply applied
  // - if they were modified locally, local changes will be rebased on top of server changes

  QString src = tempDir + "/" + CoreUtils::uuidWithoutBraces( QUuid::createUuid() );
  QString dest = projectDir + "/" + filePath;
  QString baseFile = projectDir + "/.mergin/" + filePath;

  LocalProject info = mLocalProjects.projectFromProjectId( projectId );

  // add conflict files to project dir so they can be synced
  QString conflictFile = CoreUtils::findUniquePath( CoreUtils::generateEditConflictFileName( dest, mUserInfo->username(), info.localVersion ) );

  createPathIfNotExists( src );
  createPathIfNotExists( dest );
  createPathIfNotExists( baseFile );

  QStringList diffFiles;
  for ( const auto &item : items )
    diffFiles << tempDir + "/" + item.tempFileName;

  //
  // let's first assemble server's file from our base file + diffs
  //

  if ( !QFile::copy( baseFile, src ) )
  {
    CoreUtils::log( "pull " + projectFullName, "assemble server file fail: copying failed " + baseFile + " to " + src );

    // TODO: this is a critical failure - we should abort pull
  }

  if ( !GeodiffUtils::applyDiffs( src, diffFiles ) )
  {
    CoreUtils::log( "pull " + projectFullName, "server file assembly failed: " + filePath );

    // TODO: this is a critical failure - we should abort pull
    // TODO: we could try to delete the base file and re-download it from scratch on next sync
  }
  else
  {
    CoreUtils::log( "pull " + projectFullName, "server file assembly successful: " + filePath );
  }

  //
  // now we are ready for the update of our local file
  //
  bool hasConflicts = false;

  bool res = GeodiffUtils::rebase( baseFile,
                                   src,
                                   dest,
                                   conflictFile
                                 );
  if ( res )
  {
    CoreUtils::log( "pull " + projectFullName, "geodiff rebase successful: " + filePath );
  }
  else
  {
    CoreUtils::log( "pull " + projectFullName, "geodiff rebase failed! " + filePath );

    // not good... something went wrong in rebase - we need to save the local changes
    // let's put them into a conflict file and use the server version
    hasConflicts = true;
    LocalProject localProject = mLocalProjects.projectFromProjectId( projectId );
    QString newDest = CoreUtils::findUniquePath( CoreUtils::generateConflictedCopyFileName( dest, mUserInfo->username(), localProject.localVersion ) );
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
  // finally update our base file
  //

  if ( !QFile::remove( baseFile ) )
  {
    CoreUtils::log( "pull " + projectFullName, "failed removal of old base file: " + filePath );

    // TODO: this is a critical failure - we should abort pull
  }
  if ( !QFile::rename( src, baseFile ) )
  {
    CoreUtils::log( "pull " + projectFullName, "failed rename of base file using new server content: " + filePath );

    // TODO: this is a critical failure - we should abort pull
  }
  return hasConflicts;
}

void MerginApi::finalizeProjectPull( const QString &projectId )
{
  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];
  const MerginProjectMetadata project = MerginProjectMetadata::fromJson( transaction.projectMetadata );
  const QString projectFullName = CoreUtils::getFullProjectName( project.projectNamespace, project.name );

  const QString projectDir = transaction.projectDir;
  const QString tempProjectDir = getTempProjectDir( projectFullName );

  CoreUtils::log( "pull " + projectFullName, "Running update tasks" );

  for ( const PullTask &finalizationItem : transaction.pullTasks )
  {
    switch ( finalizationItem.method )
    {
      case PullTask::Copy:
      {
        finalizeProjectPullCopy( projectFullName, projectDir, tempProjectDir, finalizationItem.filePath, finalizationItem.data );
        break;
      }

      case PullTask::CopyConflict:
      {
        // move local file to conflict file
        QString origPath = projectDir + "/" + finalizationItem.filePath;
        const LocalProject info = mLocalProjects.projectFromProjectId( projectId );
        QString newPath = CoreUtils::findUniquePath( CoreUtils::generateConflictedCopyFileName( origPath, mUserInfo->username(), info.localVersion ) );
        if ( !QFile::rename( origPath, newPath ) )
        {
          CoreUtils::log( "pull " + projectFullName, "failed rename of conflicting file: " + finalizationItem.filePath );
        }
        else
        {
          CoreUtils::log( "pull " + projectFullName, "Local file renamed due to conflict with server: " + finalizationItem.filePath );
        }
        finalizeProjectPullCopy( projectFullName, projectDir, tempProjectDir, finalizationItem.filePath, finalizationItem.data );
        break;
      }

      case PullTask::ApplyDiff:
      {
        // applying diff can result in conflicted copy too, in this case
        // we need to update gpkgSchemaChanged flag.
        const bool res = finalizeProjectPullApplyDiff( projectFullName, projectId, projectDir, tempProjectDir, finalizationItem.filePath, finalizationItem.data );
        transaction.gpkgSchemaChanged = res;
        break;
      }

      case PullTask::Delete:
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
  const int tmpFilesLeft = static_cast<int>( QDir( tempProjectDir ).entryList( QDir::NoDotAndDotDot ).count() );
  if ( tmpFilesLeft )
  {
    CoreUtils::log( "pull " + projectFullName, "Some temporary files were left - this should not happen..." );
  }

  QDir( tempProjectDir ).removeRecursively();

  // add the local project if not there yet
  if ( !mLocalProjects.projectFromProjectId( projectId ).isValid() )
  {
    // remove download in progress file
    if ( !QFile::remove( CoreUtils::downloadInProgressFilePath( transaction.projectDir ) ) )
      CoreUtils::log( QStringLiteral( "sync %1" ).arg( projectFullName ), QStringLiteral( "Failed to remove download in progress file for project name %1" ).arg( project.name ) );

    mLocalProjects.addMerginProject( projectDir, project.projectNamespace, project.name, projectId );
  }

  finishProjectSync( projectFullName, projectId, true );
}


void MerginApi::pushStartReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();
  QString projectId = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ) ).toString();

  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];
  Q_ASSERT( r == transaction.replyPushStart );

  if ( r->error() == QNetworkReply::NoError )
  {
    QByteArray data = r->readAll();

    transaction.replyPushStart->deleteLater();
    transaction.replyPushStart = nullptr;

    QList<MerginFile> files = transaction.pushQueue;
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

      if ( transaction.transactionUUID.isEmpty() )
      {
        CoreUtils::log( "push " + projectFullName, QStringLiteral( "Fail! Could not acquire transaction ID" ) );
        finishProjectSync( projectFullName, projectId, false );
      }

      CoreUtils::log( "push " + projectFullName, QStringLiteral( "Push request accepted. Transaction ID: " ) + transactionUUID );

      const MerginFile &file = files.first();
      pushFile( projectFullName, projectId, transactionUUID, file );
      emit pushFilesStarted();
    }
    else  // pushing only files to be removed
    {
      // we are done here - no upload of chunks, no request to "finish"
      // because server immediately creates a new version without starting a transaction to upload chunks

      CoreUtils::log( "push " + projectFullName, QStringLiteral( "Push request accepted and no files to upload" ) );

      transaction.projectMetadata = data;
      transaction.version = MerginProjectMetadata::fromJson( data ).version;

      finishProjectSync( projectFullName, projectId, true );
    }
  }
  else
  {
    QByteArray data = r->readAll();
    QString serverMsg = extractServerErrorMsg( data );
    if ( r->error() == QNetworkReply::OperationCanceledError )
      serverMsg = sSyncCanceledMessage;

    QString code = extractServerErrorCode( data );
    bool showLimitReachedDialog = EnumHelper::isEqual( code, ErrorCode::StorageLimitHit );

    CoreUtils::log( "push " + projectFullName, QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );

    transaction.replyPushStart->deleteLater();
    transaction.replyPushStart = nullptr;

    if ( showLimitReachedDialog )
    {
      const QList<MerginFile> files = transaction.pushQueue;
      qreal uploadSize = 0;
      for ( const MerginFile &f : files )
      {
        uploadSize += static_cast<qreal>( f.size );
      }
      emit storageLimitReached( uploadSize );

      // remove project if it was first time sync - migration
      if ( transaction.isInitialPush )
      {
        detachProjectFromMergin( projectId, false );
        deleteProject( projectId, false );
      }
    }
    else
    {
      int httpCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
      emit networkErrorOccurred( serverMsg, httpCode, projectId );
    }
    finishProjectSync( projectFullName, projectId, false );
  }
}

void MerginApi::pushFileReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();
  QString projectId = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ) ).toString();

  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];
  Q_ASSERT( r == transaction.replyPushFile );

  QStringList params = r->url().toString().split( "/" );
  const QString &transactionUUID = params.at( params.length() - 2 );
  const QString &chunkID = params.at( params.length() - 1 );
  Q_ASSERT( transactionUUID == transaction.transactionUUID );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Uploaded successfully: " ) + chunkID );

    transaction.replyPushFile->deleteLater();
    transaction.replyPushFile = nullptr;

    MerginFile currentFile = transaction.pushQueue.first();
    int chunkNo = static_cast<int>( currentFile.chunks.indexOf( chunkID ) );
    if ( chunkNo < currentFile.chunks.size() - 1 )
    {
      pushFile( projectFullName, projectId, transactionUUID, currentFile, chunkNo + 1 );
    }
    else
    {
      transaction.transferedSize += currentFile.size;

      emit syncProjectStatusChanged( projectId, static_cast<qreal>( transaction.transferedSize ) / transaction.totalSize );
      transaction.pushQueue.removeFirst();

      if ( !transaction.pushQueue.isEmpty() )
      {
        MerginFile nextFile = transaction.pushQueue.first();
        pushFile( projectFullName, projectId, transactionUUID, nextFile );
      }
      else
      {
        pushFinish( projectFullName, projectId, transactionUUID );
      }
    }
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    if ( r->error() == QNetworkReply::OperationCanceledError )
      serverMsg = sSyncCanceledMessage;

    CoreUtils::log( "push " + projectFullName, QStringLiteral( "FAILED - %1. %2" ).arg( r->errorString(), serverMsg ) );

    int httpCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    emit networkErrorOccurred( serverMsg, httpCode, projectId );

    transaction.replyPushFile->deleteLater();
    transaction.replyPushFile = nullptr;

    finishProjectSync( projectFullName, projectId, false );
  }
}

void MerginApi::pullInfoReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  const QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();
  const QString projectId = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ) ).toString();

  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];
  Q_ASSERT( r == transaction.replyPullProjectInfo );

  if ( r->error() == QNetworkReply::NoError )
  {
    const QByteArray data = r->readAll();
    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Downloaded project info." ) );

    transaction.replyPullProjectInfo->deleteLater();
    transaction.replyPullProjectInfo = nullptr;

    prepareProjectPull( projectId, data );
  }
  else
  {
    // if the operation was cancelled we finish pull, but if something failed we try fetching project info by project ID
    if ( r->error() == QNetworkReply::OperationCanceledError )
    {
      const QString serverMsg = sSyncCanceledMessage;
      const QString message = QStringLiteral( "Network API error: %1(): %2" ).arg( QStringLiteral( "projectInfo" ), r->errorString() );
      CoreUtils::log( "pull " + projectFullName, QStringLiteral( "FAILED - %1" ).arg( message ) );

      const int httpCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
      emit networkErrorOccurred( serverMsg, httpCode, projectId );

      transaction.replyPullProjectInfo->deleteLater();
      transaction.replyPullProjectInfo = nullptr;

      finishProjectSync( projectFullName, projectId, false );
    } else
    {
      const QString serverMsg = extractServerErrorMsg( r->readAll() );
      const QString message = QStringLiteral( "Network API error: %1(): %2" ).arg( QStringLiteral( "projectInfo" ), r->errorString() );
      CoreUtils::log( "pull " + projectFullName, QStringLiteral( "FAILED - %1" ).arg( message ) );
      CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Triggering info look up by project ID" ) );

      const bool withAuth = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrAuthUsed ) ).toBool();
      const QNetworkReply *reply = getProjectsDetails( {projectId}, withAuth );
      connect( reply, &QNetworkReply::finished, this, &MerginApi::getProjectsDetailsReplyFinished );
    }


  }
}

void MerginApi::prepareProjectPull( const QString &projectId, const QByteArray &data )
{
  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];

  const MerginProjectMetadata serverProject = MerginProjectMetadata::fromJson( data );

  transaction.projectMetadata = data;
  transaction.version = serverProject.version;

  const LocalProject projectInfo = mLocalProjects.projectFromProjectId( projectId );
  if ( projectInfo.isValid() )
  {
    transaction.projectDir = projectInfo.projectDir;

    // do not continue if we are already on the latest version
    if ( projectInfo.localVersion != -1 && projectInfo.localVersion == serverProject.version )
    {
      emit projectAlreadyOnLatestVersion( projectInfo.id() );
      CoreUtils::log( QStringLiteral( "Pull %1" ).arg( projectInfo.fullName() ), QStringLiteral( "Project is already on the latest version: %1" ).arg( serverProject.version ) );

      return finishProjectSync( projectInfo.fullName(), projectId, false );
    }
  }
  else
  {
    // remove any leftover temp files that could be created from previous unsuccessful download
    removeProjectsTempFolder( serverProject.projectNamespace, serverProject.name );

    // project has not been downloaded yet - we need to create a directory for it
    transaction.projectDir = CoreUtils::createUniqueProjectDirectory( mDataDir, serverProject.name );
    transaction.firstTimeDownload = true;

    // create file indicating first time download in progress
    const QString downloadInProgressFilePath = CoreUtils::downloadInProgressFilePath( transaction.projectDir );
    createPathIfNotExists( downloadInProgressFilePath );
    const QString projectFullName = CoreUtils::getFullProjectName( serverProject.projectNamespace, serverProject.name );
    if ( !CoreUtils::createEmptyFile( downloadInProgressFilePath ) )
      CoreUtils::log( QStringLiteral( "pull %1" ).arg( projectFullName ), "Unable to create temporary download in progress file" );

    CoreUtils::log( "pull " + projectFullName, QStringLiteral( "First time download - new directory: " ) + transaction.projectDir );
  }

  Q_ASSERT( !transaction.projectDir.isEmpty() );  // that would mean we do not have entry -> fail getting local files

  if ( transaction.configAllowed )
  {
    prepareDownloadConfig( projectId );
  }
  else
  {
    startProjectPull( projectId );
  }
}

void MerginApi::startProjectPull( const QString &projectId )
{
  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];

  QList<MerginFile> localFiles = getLocalProjectFiles( transaction.projectDir + "/" );
  MerginProjectMetadata serverProject = MerginProjectMetadata::fromJson( transaction.projectMetadata );
  MerginProjectMetadata oldServerProject = MerginProjectMetadata::fromCachedJson( transaction.projectDir + "/" + sMetadataFile );
  MerginConfig oldTransactionConfig = MerginConfig::fromFile( transaction.projectDir + "/" + sMerginConfigFile );
  const QString projectFullName = CoreUtils::getFullProjectName( serverProject.projectNamespace, serverProject.name );

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

  for ( const QString &filePath : transaction.diff.remoteAdded )
  {
    MerginFile file = serverProject.fileInfo( filePath );
    QList<DownloadQueueItem> items = itemsForFileChunks( file, transaction.version );
    transaction.pullTasks << PullTask( PullTask::Copy, filePath, items );
    transaction.gpkgSchemaChanged = true;
  }

  for ( const QString &filePath : transaction.diff.remoteUpdated )
  {
    MerginFile file = serverProject.fileInfo( filePath );

    // for diffable files - download and apply to the base file (without rebase)
    if ( isFileDiffable( filePath ) && file.pullCanUseDiff )
    {
      QList<DownloadQueueItem> items = itemsForFileDiffs( file );
      transaction.pullTasks << PullTask( PullTask::ApplyDiff, filePath, items );
    }
    else
    {
      QList<DownloadQueueItem> items = itemsForFileChunks( file, transaction.version );
      transaction.pullTasks << PullTask( PullTask::Copy, filePath, items );
      transaction.gpkgSchemaChanged = true;
    }
  }

  // also download files which were changed both on the server and locally (the local version will be renamed as conflicting copy)
  for ( const QString &filePath : transaction.diff.conflictRemoteUpdatedLocalUpdated )
  {
    MerginFile file = serverProject.fileInfo( filePath );

    // for diffable files - download and apply to the base file (will also do rebase)
    if ( isFileDiffable( filePath ) && file.pullCanUseDiff )
    {
      QList<DownloadQueueItem> items = itemsForFileDiffs( file );
      transaction.pullTasks << PullTask( PullTask::ApplyDiff, filePath, items );
    }
    else
    {
      QList<DownloadQueueItem> items = itemsForFileChunks( file, transaction.version );
      transaction.pullTasks << PullTask( PullTask::CopyConflict, filePath, items );
      transaction.gpkgSchemaChanged = true;
    }
  }

  // also download files which were added both on the server and locally (the local version will be renamed as conflicting copy)
  for ( const QString &filePath : transaction.diff.conflictRemoteAddedLocalAdded )
  {
    MerginFile file = serverProject.fileInfo( filePath );
    QList<DownloadQueueItem> items = itemsForFileChunks( file, transaction.version );
    transaction.pullTasks << PullTask( PullTask::CopyConflict, filePath, items );
    transaction.gpkgSchemaChanged = true;
  }

  // schedule removed files to be deleted
  for ( const QString &filePath : transaction.diff.remoteDeleted )
  {
    transaction.pullTasks << PullTask( PullTask::Delete, filePath, QList<DownloadQueueItem>() );
  }

  // prepare the download queue
  for ( const PullTask &item : transaction.pullTasks )
  {
    transaction.downloadQueue << item.data;
  }

  qint64 totalSize = 0;
  for ( const DownloadQueueItem &item : transaction.downloadQueue )
  {
    totalSize += item.size;
  }
  transaction.totalSize = static_cast<qreal>( totalSize );

  // order download queue from the largest to smallest chunks to better
  // work with parallel downloads
  std::sort(
    transaction.downloadQueue.begin(), transaction.downloadQueue.end(),
  []( const DownloadQueueItem & a, const DownloadQueueItem & b ) { return a.size > b.size; }
  );

  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "%1 of available device storage, %2 of total device storage" )
                  .arg( CoreUtils::getAvailableDeviceStorage() )
                  .arg( CoreUtils::getTotalDeviceStorage() ) );

  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "%1 update tasks, %2 items to download (total size %3 bytes)" )
                  .arg( transaction.pullTasks.count() )
                  .arg( transaction.downloadQueue.count() )
                  .arg( transaction.totalSize ) );

  emit pullFilesStarted();

  if ( transaction.downloadQueue.isEmpty() )
  {
    finalizeProjectPull( projectId );
  }
  else
  {
    while ( transaction.replyPullItems.count() < 5 && !transaction.downloadQueue.isEmpty() )
    {
      downloadNextItem( projectId );
    }
  }
}

void MerginApi::prepareDownloadConfig( const QString &projectId, const bool downloaded )
{
  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];

  MerginProjectMetadata newServerVersion = MerginProjectMetadata::fromJson( transaction.projectMetadata );

  const auto res = std::find_if( newServerVersion.files.begin(), newServerVersion.files.end(), []( const MerginFile & file )
  {
    return file.path == sMerginConfigFile;
  } );
  const bool serverContainsConfig = res != newServerVersion.files.end();

  if ( serverContainsConfig )
  {
    if ( !downloaded )
    {
      // we should have server config, but we do not have it yet
      return requestServerConfig( projectId );
    }
  }

  MerginProjectMetadata oldServerVersion = MerginProjectMetadata::fromCachedJson( transaction.projectDir + "/" + sMetadataFile );

  const auto resOld = std::find_if( oldServerVersion.files.begin(), oldServerVersion.files.end(), []( const MerginFile & file )
  {
    return file.path == sMerginConfigFile;
  } );

  const bool previousVersionContainedConfig = resOld != oldServerVersion.files.end() && !transaction.firstTimeDownload;

  if ( !transaction.config.isValid )
  {
    // if transaction is not valid (or missing), consider it as deleted
    transaction.config.downloadMissingFiles = true;
    CoreUtils::log( "MerginConfig", "No config detected" );
  }
  else if ( serverContainsConfig && previousVersionContainedConfig )
  {
    // config was there, check if there are changes
    const QString newChk = newServerVersion.fileInfo( sMerginConfigFile ).checksum;
    const QString oldChk = oldServerVersion.fileInfo( sMerginConfigFile ).checksum;

    if ( newChk == oldChk )
    {
      // config files are the same
    }
    else
    {
      // config was changed, but what changed?
      const MerginConfig oldConfig = MerginConfig::fromFile( transaction.projectDir + "/" + sMerginConfigFile );

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
    // pull like without config
    transaction.configAllowed = false;
    transaction.config.isValid = false;

    // if it would be possible to add mergin-config locally, it needs to be checked here
  }

  startProjectPull( projectId );
}

void MerginApi::requestServerConfig( const QString &projectId )
{
  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];
  const MerginProjectMetadata project = MerginProjectMetadata::fromJson( transaction.projectMetadata );
  const QString projectFullName = CoreUtils::getFullProjectName( project.projectNamespace, project.name );

  QUrl url( mApiRoot + QStringLiteral( "/v1/project/raw/" ) + projectFullName );
  QUrlQuery query;

  query.addQueryItem( "file", sMerginConfigFile.toUtf8().toPercentEncoding() );
  query.addQueryItem( "version", QStringLiteral( "v%1" ).arg( transaction.version ) );
  url.setQuery( query );

  QNetworkRequest request = getDefaultRequest();
  request.setUrl( url );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ), projectId );

  Q_ASSERT( !transaction.replyPullServerConfig );
  transaction.replyPullServerConfig = mManager->get( request );
  connect( transaction.replyPullServerConfig, &QNetworkReply::finished, this, &MerginApi::cacheServerConfig );

  CoreUtils::log( "pull " + projectFullName, QStringLiteral( "Requesting mergin config: " ) + url.toString() );
}

QList<DownloadQueueItem> MerginApi::itemsForFileChunks( const MerginFile &file, const int version )
{
  QList<DownloadQueueItem> lst;
  qint64 from = 0;
  while ( from < file.size )
  {
    const qint64 size = qMin( UPLOAD_CHUNK_SIZE, file.size - from );
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
  CoreUtils::log( QStringLiteral( "MerginFile" ), QStringLiteral( "requested findFile() for non-existent file: %1" ).arg( filePath ) );
  return {};
}


void MerginApi::pushInfoReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();
  QString projectId = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ) ).toString();

  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];
  Q_ASSERT( r == transaction.replyPushProjectInfo );

  if ( r->error() == QNetworkReply::NoError )
  {
    QString url = r->url().toString();
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Downloaded project info." ) );
    QByteArray data = r->readAll();

    transaction.replyPushProjectInfo->deleteLater();
    transaction.replyPushProjectInfo = nullptr;

    LocalProject projectInfo = mLocalProjects.projectFromProjectId( projectId );
    transaction.projectDir = projectInfo.projectDir;
    Q_ASSERT( !transaction.projectDir.isEmpty() );

    // get the latest server version from our reply (we do not update it in LocalProjectsManager though... I guess we don't need to)
    MerginProjectMetadata serverProject = MerginProjectMetadata::fromJson( data );

    // now let's figure a key question: are we on the most recent version of the project
    // if we're about to do upload? because if not, we need to do pull first
    if ( projectInfo.isValid() && projectInfo.localVersion != -1 && projectInfo.localVersion < serverProject.version )
    {
      CoreUtils::log( "push " + projectFullName, QStringLiteral( "Need pull first: local version %1 | server version %2" )
                      .arg( projectInfo.localVersion ).arg( serverProject.version ) );
      transaction.pullBeforePush = true;
      prepareProjectPull( projectId, data );
      return;
    }

    QList<MerginFile> localFiles = getLocalProjectFiles( transaction.projectDir + "/" );
    MerginProjectMetadata oldServerProject = MerginProjectMetadata::fromCachedJson( transaction.projectDir + "/" + sMetadataFile );

    // Cache mergin-config, since we are on the most recent version, it is sufficient to just read the local version
    if ( transaction.configAllowed )
    {
      transaction.config = MerginConfig::fromFile( transaction.projectDir + "/" + sMerginConfigFile );
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
    for ( const QString &filePath : transaction.diff.localAdded )
    {
      MerginFile merginFile = findFile( filePath, localFiles );
      merginFile.chunks = generateChunkIdsForSize( merginFile.size );
      addedMerginFiles.append( merginFile );
    }

    for ( const QString &filePath : transaction.diff.localUpdated )
    {
      MerginFile merginFile = findFile( filePath, localFiles );
      merginFile.chunks = generateChunkIdsForSize( merginFile.size );

      if ( isFileDiffable( filePath ) )
      {
        // try to create a diff
        QString diffName;
        int geodiffRes = GeodiffUtils::createChangeset( transaction.projectDir, filePath, diffName );
        QString diffPath = transaction.projectDir + "/.mergin/" + diffName;
        QString basePath = transaction.projectDir + "/.mergin/" + filePath;

        if ( geodiffRes == GEODIFF_SUCCESS )
        {
          QByteArray checksumDiff = CoreUtils::calculateChecksum( diffPath );

          // TODO: this is ugly. our base file may not need to have the same checksum as the server's
          // base file (because each of them have applied the diff independently) so we have to fake it
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

    for ( const QString &filePath : transaction.diff.localDeleted )
    {
      MerginFile merginFile = findFile( filePath, serverProject.files );
      deletedMerginFiles.append( merginFile );
    }

    if ( addedMerginFiles.isEmpty() && updatedMerginFiles.isEmpty() && deletedMerginFiles.isEmpty() )
    {
      // if nothing has changed, there is no point to even start upload transaction
      transaction.projectMetadata = data;
      transaction.version = MerginProjectMetadata::fromJson( data ).version;

      finishProjectSync( projectFullName, projectId, true );
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
    for ( const MerginFile &file : filesToUpload )
    {
      if ( !file.diffName.isEmpty() )
        totalSize += file.diffSize;
      else
        totalSize += file.size;
    }

    CoreUtils::log( "push " + projectFullName, QStringLiteral( "%1 items to upload (total size %2 bytes)" )
                    .arg( filesToUpload.count() ).arg( totalSize ) );

    transaction.totalSize = static_cast<qreal>( totalSize );
    transaction.pushQueue = filesToUpload;
    transaction.pushDiffFiles = diffFiles;

    QJsonObject json;
    json.insert( QStringLiteral( "changes" ), changes );
    json.insert( QStringLiteral( "version" ), QString( "v%1" ).arg( serverProject.version ) );
    QJsonDocument jsonDoc;
    jsonDoc.setObject( json );

    pushStart( projectFullName, projectId, jsonDoc.toJson( QJsonDocument::Compact ) );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    if ( r->error() == QNetworkReply::OperationCanceledError )
      serverMsg = sSyncCanceledMessage;

    QString message = QStringLiteral( "Network API error: %1(): %2" ).arg( QStringLiteral( "projectInfo" ), r->errorString() );
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "FAILED - %1" ).arg( message ) );

    int httpCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    emit networkErrorOccurred( serverMsg, httpCode, projectId );

    transaction.replyPushProjectInfo->deleteLater();
    transaction.replyPushProjectInfo = nullptr;

    finishProjectSync( projectFullName, projectId, false );
  }
}

void MerginApi::pushFinishReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  const QString projectFullName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectFullName ) ).toString();
  const QString projectId = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ) ).toString();

  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  TransactionStatus &transaction = mTransactionalStatus[projectId];
  Q_ASSERT( r == transaction.replyPushFinish );

  if ( r->error() == QNetworkReply::NoError )
  {
    Q_ASSERT( mTransactionalStatus.contains( projectId ) );
    const QByteArray data = r->readAll();
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Transaction finish accepted" ) );

    transaction.replyPushFinish->deleteLater();
    transaction.replyPushFinish = nullptr;

    transaction.projectMetadata = data;
    transaction.version = MerginProjectMetadata::fromJson( data ).version;

    //  a new diffable files suppose to have their base file copies in .mergin
    for ( const QString &filePath : transaction.diff.localAdded )
    {
      if ( isFileDiffable( filePath ) )
      {
        QString baseFile = transaction.projectDir + "/.mergin/" + filePath;
        createPathIfNotExists( baseFile );

        QString sourcePath = transaction.projectDir + "/" + filePath;
        if ( !QFile::copy( sourcePath, baseFile ) )
        {
          CoreUtils::log( "push " + projectFullName, "failed to copy new base file for: " + filePath );
        }
      }
    }

    // clean up diff-related files
    const auto diffFiles = transaction.pushDiffFiles;
    for ( const MerginFile &merginFile : diffFiles )
    {
      QString diffPath = transaction.projectDir + "/.mergin/" + merginFile.diffName;

      // update base file (unmodified file that should be equivalent to the server)
      QString basePath = transaction.projectDir + "/.mergin/" + merginFile.path;
      const bool res = GeodiffUtils::applyChangeset( basePath, diffPath );
      if ( res )
      {
        CoreUtils::log( "push " + projectFullName, QString( "Applied %1 to base file of %2" ).arg( merginFile.diffName, merginFile.path ) );
      }
      else
      {
        CoreUtils::log( "push " + projectFullName, QString( "Failed to apply changeset %1 to base file %2 - error %3" ).arg( diffPath ).arg( basePath ).arg( res ) );
      }

      // remove temporary diff files
      if ( !QFile::remove( diffPath ) )
        CoreUtils::log( "push " + projectFullName, "Failed to remove diff: " + diffPath );
    }

    finishProjectSync( projectFullName, projectId, true );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    if ( r->error() == QNetworkReply::OperationCanceledError )
      serverMsg = sSyncCanceledMessage;

    const QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "pushFinish" ), r->errorString(), serverMsg );
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "FAILED - %1" ).arg( message ) );

    const int httpCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    emit networkErrorOccurred( serverMsg, httpCode, projectId );

    // remove temporary diff files
    const auto diffFiles = transaction.pushDiffFiles;
    for ( const MerginFile &merginFile : diffFiles )
    {
      QString diffPath = transaction.projectDir + "/.mergin/" + merginFile.diffName;
      if ( !QFile::remove( diffPath ) )
        CoreUtils::log( "push " + projectFullName, "Failed to remove diff: " + diffPath );
    }

    transaction.replyPushFinish->deleteLater();
    transaction.replyPushFinish = nullptr;

    finishProjectSync( projectFullName, projectId, false );
  }
}

void MerginApi::pushCancelReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  const QString projectId = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ) ).toString();
  const QString projectFullName = mLocalProjects.projectFromProjectId( projectId ).fullName();

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "Transaction canceled" ) );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    const QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "uploadCancel" ), r->errorString(), serverMsg );
    CoreUtils::log( "push " + projectFullName, QStringLiteral( "FAILED - %1" ).arg( message ) );
  }

  emit pushCanceled( projectId );

  r->deleteLater();
}

void MerginApi::getUserInfoFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "user info", QStringLiteral( "Success" ) );
    const QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      const QJsonObject docObj = doc.object();
      mUserInfo->setFromJson( docObj );
      if ( mServerType == MerginServerType::OLD )
      {
        mWorkspaceInfo->setFromJson( docObj );
      }

      switch ( mUserAuth->authMethod() )
      {
        case MerginUserAuth::AuthMethod::SSO:
          mUserAuth->blockSignals( true );
          mUserAuth->setLogin( mUserInfo->email() );
          mUserAuth->saveData();
          mUserAuth->blockSignals( false );
          break;
        case MerginUserAuth::AuthMethod::Password:
          break;
      }
    }
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    const QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "getUserInfo" ), r->errorString(), serverMsg );
    CoreUtils::log( "user info", QStringLiteral( "FAILED - %1" ).arg( message ) );

    // This is an ugly fix for #3261: if the user was logged in, but the token was already expired
    // (e.g. when starting the app the next day), the flow of network requests and handlers gets
    // confused because of mAuthLoopEvent involved when re-authenticating user to get new token.
    // We end up requesting user info even with expired token, which of course fails with HTTP code 401
    // and user gets "Authentication information is missing or invalid." notification - this code
    // prevents that. The correct solution is to get rid of the QEventLoop and to have more rigorous
    // flow of network requests.
    static bool firstTimeExpiredTokenAnd401 = true;
    if ( firstTimeExpiredTokenAnd401 && r->attribute( QNetworkRequest::HttpStatusCodeAttribute ) == 401 &&
         !mUserAuth->authToken().isEmpty() && mUserAuth->tokenExpiration() < QDateTime::currentDateTimeUtc() )
    {
      firstTimeExpiredTokenAnd401 = false;
    }
    else
    {
      emit networkErrorOccurred( serverMsg );
    }
  }

  emit userInfoReplyFinished();

  r->deleteLater();
}

void MerginApi::getWorkspaceInfoReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "workspace info", QStringLiteral( "Success" ) );
    const QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      const QJsonObject docObj = doc.object();
      mWorkspaceInfo->setFromJson( docObj );

      emit getWorkspaceInfoFinished();
    }
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    const QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "getWorkspaceInfo" ), r->errorString(), serverMsg );
    CoreUtils::log( "workspace info", QStringLiteral( "FAILED - %1" ).arg( message ) );
    mWorkspaceInfo->clear();
    emit networkErrorOccurred( serverMsg );
  }

  r->deleteLater();
}

bool MerginApi::hasLocalChanges(
  const QList<MerginFile> &oldServerFiles,
  const QList<MerginFile> &localFiles,
  const QString &projectDir,
  const MerginConfig &config
)
{
  QList<MerginFile> resolvedOldServerFiles;

  if ( config.isValid ) // if a config was set, selective sync is supported
  {
    for ( const MerginFile &file : oldServerFiles )
    {
      if ( !excludeFromSync( file.path, config ) )
        resolvedOldServerFiles.append( file );
    }
  }
  else
  {
    resolvedOldServerFiles = oldServerFiles;
  }

  if ( localFiles.count() != resolvedOldServerFiles.count() )
  {
    return true;
  }

  QHash<QString, MerginFile> oldServerFilesMap;

  for ( const MerginFile &file : resolvedOldServerFiles )
  {
    oldServerFilesMap.insert( file.path, file );
  }

  for ( const MerginFile &localFile : localFiles )
  {
    QString filePath = localFile.path;
    const bool hasOldServer = oldServerFilesMap.contains( localFile.path );

    if ( !hasOldServer )
    {
      // L-A
      return true;
    }

    const QString chkOld = oldServerFilesMap.value( localFile.path ).checksum;
    const QString chkLocal = localFile.checksum;

    if ( chkOld != chkLocal )
    {
      if ( isFileDiffable( filePath ) )
      {
        // we need to do a diff here to figure out whether the file is actually changed or not
        // because the real content may be the same although the checksums do not match
        // e.g. when GPKG is opened, its header is updated and therefore lastModified timestamp/checksum is updated as well.
        if ( GeodiffUtils::hasPendingChanges( projectDir, filePath ) )
        {
          // L-U
          return true;
        }
      }
      else
      {
        // L-U
        return true;
      }
    }
  }

  // We know that the number of local files and old server is the same
  // And also that all local files has old file counterpart
  // So it is not possible that there is deleted local file at this point.
  return false;
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

  for ( const MerginFile &file : newServerFiles )
  {
    newServerFilesMap.insert( file.path, file );
  }
  for ( const MerginFile &file : oldServerFiles )
  {
    oldServerFilesMap.insert( file.path, file );
  }

  for ( const MerginFile &localFile : localFiles )
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
  for ( const MerginFile &file : newServerFilesMap )
  {
    bool hasOldServer = oldServerFilesMap.contains( file.path );

    if ( hasOldServer )
    {
      if ( oldServerFilesMap.value( file.path ).checksum == file.checksum )
      {
        // L-D
        if ( allowConfig )
        {
          bool shouldBeExcludedFromSync = excludeFromSync( file.path, config );
          if ( shouldBeExcludedFromSync )
          {
            continue;
          }

          // check if we should download missing files that were previously ignored (e.g. selective sync has been disabled)
          if ( config.downloadMissingFiles &&
               lastSyncConfig.isValid &&
               excludeFromSync( file.path, lastSyncConfig ) )
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
        if ( excludeFromSync( file.path, config ) )
        {
          continue;
        }
      }
      diff.remoteAdded << file.path;
    }

    if ( hasOldServer )
      oldServerFilesMap.remove( file.path );
  }

  /*
  for ( MerginFile file : oldServerFilesMap )
  {
    // R-D/L-D
    // TODO: need to do anything?
  }
  */

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
    project.remoteError = QString::number( proj.value( QStringLiteral( "error" ) ).toInt( 0 ) ); // error code
    return project;
  }

  project.projectName = proj.value( QStringLiteral( "name" ) ).toString();
  project.projectNamespace = proj.value( QStringLiteral( "namespace" ) ).toString();
  project.projectId = proj.value( QStringLiteral( "id" ) ).toString();

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

  const QDateTime updated = QDateTime::fromString( proj.value( QStringLiteral( "updated" ) ).toString(), Qt::ISODateWithMs ).toUTC();
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
    return {};

  QJsonObject object = doc.object();
  MerginProjectsList result;

  if ( object.contains( "projects" ) && object.value( "projects" ).isArray() ) // listProjects API
  {
    const QJsonArray vArray = object.value( "projects" ).toArray();

    for ( auto it = vArray.constBegin(); it != vArray.constEnd(); ++it )
    {
      result << parseProjectMetadata( it->toObject() );
    }
  }
  else if ( !object.isEmpty() ) // listProjectsByName API returns projects as separate objects not in array
  {
    for ( auto it = object.begin(); it != object.end(); ++it )
    {
      MerginProject project = parseProjectMetadata( it->toObject() );
      if ( !project.remoteError.isEmpty() )
      {
        // add project namespace/name from object name in case of error
        CoreUtils::extractProjectName( it.key(), project.projectNamespace, project.projectName );
      }
      result << project;
    }
  }
  return result;
}

void MerginApi::refreshAuthToken()
{
  switch ( mUserAuth->authMethod() )
  {
    case MerginUserAuth::AuthMethod::SSO:
      // refresh tokens not implemented yet
      return;
    case MerginUserAuth::AuthMethod::Password:
      if ( !mUserAuth->hasAuthData() ||
           mUserAuth->authToken().isEmpty() )
      {
        CoreUtils::log( QStringLiteral( "Auth" ), QStringLiteral( "Can not refresh token, missing credentials" ) );
        return;
      }

      if ( mUserAuth->tokenExpiration() < QDateTime::currentDateTimeUtc() )
      {
        CoreUtils::log( QStringLiteral( "Auth" ), QStringLiteral( "Token has expired, requesting new one" ) );
        authorize( mUserAuth->login(), mUserAuth->password() );
        mAuthLoopEvent.exec();
      }
  }
}

QStringList MerginApi::generateChunkIdsForSize( const qint64 fileSize )
{
  const qreal rawNoOfChunks = static_cast<qreal>( fileSize ) / UPLOAD_CHUNK_SIZE;
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

  for ( const MerginFile &file : files )
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
    for ( const QString &id : file.chunks )
    {
      chunksJson.append( id );
    }
    fileObject.insert( "chunks", chunksJson );
    jsonArray.append( fileObject );
  }
  return jsonArray;
}

void MerginApi::finishProjectSync( const QString &projectFullName, const QString &projectId, const bool syncSuccessful )
{
  Q_ASSERT( mTransactionalStatus.contains( projectId ) );
  const TransactionStatus &transaction = mTransactionalStatus[projectId];

  emit syncProjectStatusChanged( projectId, -1 );   // -1 means there's no sync going on

  if ( syncSuccessful )
  {
    // update the local metadata file
    writeData( transaction.projectMetadata, transaction.projectDir + "/" + sMetadataFile );

    // update info of local projects
    mLocalProjects.updateLocalVersion( projectId, transaction.version );

    CoreUtils::log( "sync " + projectFullName, QStringLiteral( "### Finished ###  New project version: %1\n" ).arg( transaction.version ) );
  }
  else
  {
    CoreUtils::log( "sync " + projectFullName, QStringLiteral( "### FAILED ###\n" ) );
  }

  const bool pullBeforePush = transaction.pullBeforePush;
  QString projectDir = transaction.projectDir;  // keep it before the transaction gets removed
  const ProjectDiff diff = transaction.diff;
  const int newVersion = syncSuccessful ? transaction.version : -1;

  if ( transaction.gpkgSchemaChanged || projectFileHasBeenUpdated( diff ) )
  {
    emit projectReloadNeededAfterSync( projectId );
  }

  mTransactionalStatus.remove( projectId );

  if ( pullBeforePush )
  {
    CoreUtils::log( "sync " + projectFullName, QStringLiteral( "Continue with push after pull" ) );
    // we're done only with the download part before the actual upload - so let's continue with upload
    pushProject( projectFullName, projectId );
  }
  else
  {
    emit syncProjectFinished( projectId, syncSuccessful, newVersion );

    if ( syncSuccessful )
    {
      emit projectDataChanged( projectFullName, projectId );
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

bool MerginApi::updateCachedProjectRole( const QString &projectId, const QString &newRole ) const
{
  const LocalProject project = mLocalProjects.projectFromProjectId( projectId );
  if ( !project.isValid() )
  {
    return false;
  }

  const QString metadataPath = project.projectDir + "/" + sMetadataFile;
  return CoreUtils::replaceValueInJson( metadataPath, "role", newRole );
}

void MerginApi::createPathIfNotExists( const QString &filePath )
{
  const QDir dir;
  if ( !dir.exists( mDataDir ) )
    if ( !dir.mkpath( mDataDir ) )
    {
      qDebug() << QString( "Failed to create directory for %1" ).arg( mDataDir );
    };

  const QFileInfo newFile( filePath );
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
  return sIgnoreExtensions.contains( info.suffix() ) || sIgnoreFiles.contains( info.fileName() ) || info.filePath().contains( sMetadataFolder + "/" );
}

bool MerginApi::excludeFromSync( const QString &filePath, const MerginConfig &config )
{
  if ( config.isValid && config.selectiveSyncEnabled )
  {
    const QFileInfo info( filePath );

    const bool isExcludedFormat = sIgnoreImageExtensions.contains( info.suffix().toLower() );

    if ( !isExcludedFormat )
      return false;

    if ( config.selectiveSyncDir.isEmpty() )
    {
      return true; // we are ignoring photos in the entire project
    }
    if ( filePath.startsWith( config.selectiveSyncDir ) )
    {
      return true; // we are ignoring photo in subfolder
    }
  }
  return false;
}

QSet<QString> MerginApi::listFiles( const QString &projectPath )
{
  QSet<QString> files;
  QDirIterator it( projectPath, QStringList() << QStringLiteral( "*" ), QDir::Files, QDirIterator::Subdirectories );
  while ( it.hasNext() )
  {
    it.next();
    if ( !isInIgnore( it.fileInfo() ) )
    {
      files << it.filePath().replace( projectPath, "" );
    }
  }
  return files;
}

void MerginApi::deleteAccount()
{
  if ( !validateAuth() || mApiVersionStatus != MerginApiStatus::OK )
  {
    return;
  }

  QNetworkRequest request = getDefaultRequest();
  const QUrl url( mApiRoot + QStringLiteral( "/v1/user" ) );
  request.setUrl( url );
  const QNetworkReply *reply = mManager->deleteResource( request );
  connect( reply, &QNetworkReply::finished, this, [this] { this->deleteAccountFinished();} );
  CoreUtils::log( "delete account " + mUserInfo->username(), QStringLiteral( "Requesting account deletion: " ) + url.toString() );
}

void MerginApi::deleteAccountFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "delete account " + mUserInfo->username(), QStringLiteral( "Success" ) );

    // remove all local projects from the device
    mLocalProjects.projects().clear();

    clearAuth();

    emit accountDeleted( true );
  }
  else
  {
    const int statusCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    const QString serverMsg = extractServerErrorMsg( r->readAll() );
    CoreUtils::log( "delete account " + mUserInfo->username(), QStringLiteral( "FAILED - %1 %2. %3" ).arg( statusCode ).arg( r->errorString() ).arg( serverMsg ) );
    if ( statusCode == 422 )
    {
      emit userIsAnOrgOwnerError();
    }
    else
    {
      emit networkErrorOccurred( serverMsg );
    }

    emit accountDeleted( false );
  }

  r->deleteLater();
}

void MerginApi::getServerConfig()
{
  QNetworkRequest request = getDefaultRequest();
  const QString urlString = mApiRoot + QStringLiteral( "/config" );
  const QUrl url( urlString );
  request.setUrl( url );

  const QNetworkReply *reply = mManager->get( request );

  connect( reply, &QNetworkReply::finished, this, &MerginApi::getServerConfigReplyFinished );
  CoreUtils::log( "Config", QStringLiteral( "Requesting server configuration: " ) + url.toString() );
}

void MerginApi::getServerConfigReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "Config", QStringLiteral( "Success" ) );
    const QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isObject() )
    {
      const QString serverType = doc.object().value( QStringLiteral( "server_type" ) ).toString();
      const QString apiVersion = doc.object().value( QStringLiteral( "version" ) ).toString();
      int major = -1;
      int minor = -1;
      const bool validVersion = parseVersion( apiVersion, major, minor );

      if ( !validVersion )
      {
        CoreUtils::log( QStringLiteral( "Server version" ), QStringLiteral( "Cannot parse server version" ) );
      }

      if ( serverType == QStringLiteral( "ee" ) )
      {
        setServerType( MerginServerType::EE );
        if ( validVersion )
        {
          CoreUtils::log( QStringLiteral( "Server version" ), QStringLiteral( "%1.%2 EE" ).arg( major ).arg( minor ) );
        }
      }
      else if ( serverType == QStringLiteral( "ce" ) )
      {
        setServerType( MerginServerType::CE );
        if ( validVersion )
        {
          CoreUtils::log( QStringLiteral( "Server version" ), QStringLiteral( "%1.%2 CE" ).arg( major ).arg( minor ) );
        }
      }
      else if ( serverType == QStringLiteral( "saas" ) )
      {
        setServerType( MerginServerType::SAAS );
        if ( validVersion )
        {
          CoreUtils::log( QStringLiteral( "Server version" ), QStringLiteral( "%1.%2 SAAS" ).arg( major ).arg( minor ) );
        }
      }

      // will be dropped support for old servers (mostly CE servers without workspaces)
      if ( ( MINIMUM_SERVER_VERSION_MAJOR == major && MINIMUM_SERVER_VERSION_MINOR > minor ) || MINIMUM_SERVER_VERSION_MAJOR > major )
      {
        emit migrationRequested( QString( "%1.%2" ).arg( major ).arg( minor ) );
      }

      const bool ssoEnabled = doc.object().value( QStringLiteral( "sso_enabled" ) ).toBool( false );
      setApiSupportsSso( ssoEnabled );
    }
  }
  else
  {
    const int statusCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    if ( statusCode == 404 ) // legacy (old) server
    {
      setServerType( MerginServerType::OLD );
      emit migrationRequested( "legacy" );
    }
    else
    {
      QString serverMsg = extractServerErrorMsg( r->readAll() );
      const QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "getServerType" ), r->errorString(), serverMsg );
      emit networkErrorOccurred( serverMsg );
      CoreUtils::log( "server type", QStringLiteral( "FAILED - %1" ).arg( message ) );
    }
  }

  r->deleteLater();
}

MerginServerType::ServerType MerginApi::serverType() const
{
  return mServerType;
}

void MerginApi::setServerType( const MerginServerType::ServerType &serverType )
{
  if ( mServerType != serverType )
  {
    if ( mServerType == MerginServerType::OLD && serverType == MerginServerType::SAAS )
    {
      emit serverWasUpgraded();
    }

    mServerType = serverType;
    QSettings settings;
    settings.beginGroup( QStringLiteral( "Input/" ) );
    settings.setValue( QStringLiteral( "serverType" ), mServerType );
    settings.endGroup();
    emit serverTypeChanged();
    emit apiSupportsWorkspacesChanged();
  }
}

void MerginApi::listWorkspaces()
{
  if ( !validateAuth() || mApiVersionStatus != MerginApiStatus::OK )
  {
    emit listWorkspacesFailed();
    return;
  }

  const QUrl url( mApiRoot + QStringLiteral( "/v1/workspaces" ) );
  QNetworkRequest request = getDefaultRequest( mUserAuth->hasAuthData() );
  request.setUrl( url );

  const QNetworkReply *reply = mManager->get( request );
  CoreUtils::log( "list workspaces", QStringLiteral( "Requesting: " ) + url.toString() );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::listWorkspacesReplyFinished );
}

void MerginApi::listWorkspacesReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "list workspaces", QStringLiteral( "Success" ) );
    const QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isArray() )
    {
      QMap<int, QString> workspaces;
      const QJsonArray array = doc.array();
      for ( auto it = array.constBegin(); it != array.constEnd(); ++it )
      {
        QJsonObject ws = it->toObject();
        workspaces.insert( ws.value( QStringLiteral( "id" ) ).toInt(), ws.value( QStringLiteral( "name" ) ).toString() );
      }

      mUserInfo->updateWorkspacesList( workspaces );

      emit listWorkspacesFinished( workspaces );
    }
    else
    {
      emit listWorkspacesFailed();
    }
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    const QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "listWorkspaces" ), r->errorString(), serverMsg );
    CoreUtils::log( "list workspaces", QStringLiteral( "FAILED - %1" ).arg( message ) );
    emit networkErrorOccurred( serverMsg );
    emit listWorkspacesFailed();
  }

  r->deleteLater();
}

void MerginApi::listInvitations()
{
  if ( !validateAuth() || mApiVersionStatus != MerginApiStatus::OK )
  {
    emit listInvitationsFailed();
    return;
  }

  const QUrl url( mApiRoot + QStringLiteral( "/v1/workspace/invitations" ) );
  QNetworkRequest request = getDefaultRequest( mUserAuth->hasAuthData() );
  request.setUrl( url );

  const QNetworkReply *reply = mManager->get( request );
  CoreUtils::log( "list invitations", QStringLiteral( "Requesting: " ) + url.toString() );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::listInvitationsReplyFinished );
}

void MerginApi::listInvitationsReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "list invitations", QStringLiteral( "Success" ) );
    const QJsonDocument doc = QJsonDocument::fromJson( r->readAll() );
    if ( doc.isArray() )
    {
      QList<MerginInvitation> invitations;
      const QJsonArray array = doc.array();
      for ( auto it = array.constBegin(); it != array.constEnd(); ++it )
      {
        MerginInvitation invite = MerginInvitation::fromJsonObject( it->toObject() );
        invitations.append( invite );
      }

      emit listInvitationsFinished( invitations );
    }
    else
    {
      emit listInvitationsFailed();
    }
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    const QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "listInvitations" ), r->errorString(), serverMsg );
    CoreUtils::log( "list invitations", QStringLiteral( "FAILED - %1" ).arg( message ) );
    emit networkErrorOccurred( serverMsg );
    emit listInvitationsFailed();
  }

  r->deleteLater();
}

void MerginApi::processInvitation( const QString &uuid, const bool accept )
{
  if ( !validateAuth() || mApiVersionStatus != MerginApiStatus::OK )
  {
    emit processInvitationFailed();
    return;
  }

  QNetworkRequest request = getDefaultRequest( true );
  const QString urlString = mApiRoot + QStringLiteral( "/v1/workspace/invitation/%1" ).arg( uuid );
  const QUrl url( urlString );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrAcceptFlag ), accept );

  QJsonDocument jsonDoc;
  QJsonObject jsonObject;
  jsonObject.insert( QStringLiteral( "accept" ), accept );
  jsonDoc.setObject( jsonObject );
  const QByteArray json = jsonDoc.toJson( QJsonDocument::Compact );
  const QNetworkReply *reply = mManager->post( request, json );
  CoreUtils::log( "process invitation", QStringLiteral( "Requesting: " ) + url.toString() );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::processInvitationReplyFinished );
}

void MerginApi::processInvitationReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  const bool accept = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrAcceptFlag ) ).toBool();

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "process invitation", QStringLiteral( "Success" ) );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    const QString message = QStringLiteral( "Network API error: %1(): %2. %3" ).arg( QStringLiteral( "processInvitation" ), r->errorString(), serverMsg );
    CoreUtils::log( "process invitation", QStringLiteral( "FAILED - %1" ).arg( message ) );
    emit networkErrorOccurred( serverMsg );
    emit processInvitationFailed();
  }

  emit processInvitationFinished( accept );

  r->deleteLater();
}

bool MerginApi::createWorkspace( const QString &workspaceName )
{
  if ( !validateAuth() )
  {
    // this should never happen, we shouldn't be able to see the page to create new workspace without authorization
    emit notifyError( tr( "Please login again to create new workspace!" ) );
    return false;
  }

  if ( mApiVersionStatus != MerginApiStatus::OK )
  {
    return false;
  }

  if ( !CoreUtils::isValidName( workspaceName ) )
  {
    emit notifyError( tr( "Workspace name contains invalid characters" ) );
    return false;
  }

  QNetworkRequest request = getDefaultRequest();
  const QUrl url( mApiRoot + QString( "/v1/workspace" ) );
  request.setUrl( url );
  request.setRawHeader( "Content-Type", "application/json" );
  request.setRawHeader( "Accept", "application/json" );
  request.setAttribute( static_cast<QNetworkRequest::Attribute>( AttrWorkspaceName ), workspaceName );

  QJsonDocument jsonDoc;
  QJsonObject jsonObject;
  jsonObject.insert( QStringLiteral( "name" ), workspaceName );
  jsonDoc.setObject( jsonObject );
  const QByteArray json = jsonDoc.toJson( QJsonDocument::Compact );

  const QNetworkReply *reply = mManager->post( request, json );
  connect( reply, &QNetworkReply::finished, this, &MerginApi::createWorkspaceReplyFinished );
  CoreUtils::log( "create " + workspaceName, QStringLiteral( "Requesting workspace creation: " ) + url.toString() );

  return true;
}

void MerginApi::signOut()
{
  CoreUtils::log( QStringLiteral( "Auth" ), QStringLiteral( "User about to sign out" ) );
  clearAuth();
}

void MerginApi::refreshUserData()
{
  getUserInfo();

  if ( apiSupportsWorkspaces() )
  {
    getWorkspaceInfo();
    // getServiceInfo is called automatically when workspace info finishes
  }
  else if ( mServerType == MerginServerType::OLD )
  {
    getServiceInfo();
  }
}

void MerginApi::createWorkspaceReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  const QString workspaceName = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrWorkspaceName ) ).toString();

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "create " + workspaceName, QStringLiteral( "Success" ) );
    emit workspaceCreated( workspaceName );
  }
  else
  {
    QString serverMsg = extractServerErrorMsg( r->readAll() );
    const QString message = QStringLiteral( "FAILED - %1: %2" ).arg( r->errorString(), serverMsg );
    CoreUtils::log( "create " + workspaceName, message );

    const int httpCode = r->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();

    if ( httpCode == 409 )
    {
      emit networkErrorOccurred( tr( "Workspace %1 already exists" ).arg( workspaceName ), httpCode );
    }
    else
    {
      emit networkErrorOccurred( serverMsg, httpCode );
    }
  }
  r->deleteLater();
}

bool MerginApi::apiSupportsWorkspaces() const
{
  if ( mServerType == MerginServerType::SAAS || mServerType == MerginServerType::EE )
  {
    return true;
  }
  return false;
}

DownloadQueueItem::DownloadQueueItem( QString fp, const qint64 s, const int v, const qint64 rf, const qint64 rt, const bool diff )
  : filePath( std::move( fp ) ), size( s ), version( v ), rangeFrom( rf ), rangeTo( rt ), downloadDiff( diff )
{
  tempFileName = CoreUtils::uuidWithoutBraces( QUuid::createUuid() );
}

void MerginApi::reloadProjectRole( const QString &projectId )
{
  if ( projectId.isEmpty() )
    return;

  //withAuth depends on whether user is logged in or not
  const QNetworkReply *reply = getProjectDetails( projectId, mUserAuth->hasAuthData() );
  if ( !reply )
    return;

  connect( reply, &QNetworkReply::finished, this, &MerginApi::reloadProjectRoleReplyFinished );
}

void MerginApi::reloadProjectRoleReplyFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  const QString projectId = r->request().attribute( static_cast<QNetworkRequest::Attribute>( AttrProjectId ) ).toString();
  const QString cachedRole = getCachedProjectRole( projectId );

  if ( r->error() == QNetworkReply::NoError )
  {
    const QByteArray data = r->readAll();
    const MerginProjectMetadata serverProject = MerginProjectMetadata::fromJson( data );
    const QString role = serverProject.role;

    if ( role != cachedRole )
    {
      if ( updateCachedProjectRole( projectId, role ) )
        emit projectRoleUpdated( projectId, role );
    }
  }
  else
  {
    CoreUtils::log( "Metadata", QString( "Failed to update cached role for project %1 - likely due to missing auth or you are offline" ).arg( projectId ) );
  }

  r->deleteLater();
}

QString MerginApi::getCachedProjectRole( const QString &projectId ) const
{
  if ( projectId.isEmpty() )
    return {};

  const QString projectDir = mLocalProjects.projectFromProjectId( projectId ).projectDir;

  if ( projectDir.isEmpty() )
    return {};

  MerginProjectMetadata cachedProjectMetadata = MerginProjectMetadata::fromCachedJson( projectDir + "/" + sMetadataFile );

  return cachedProjectMetadata.role;
}

void MerginApi::startSsoFlow( const QString &clientId )
{
  CoreUtils::log( "SSO", QStringLiteral( "Starting SSO flow for clientId: %1" ).arg( clientId ) );
  mOauth2Flow.setAuthorizationUrl( QUrl( mApiRoot + QStringLiteral( "/v2/sso/authorize" ) ) );
  // TODO Qt 6.9: use setTokenUrl() in the abstract parent class instead.
  mOauth2Flow.setAccessTokenUrl( QUrl( mApiRoot + QStringLiteral( "/v2/sso/token" ) ) );
  mOauth2Flow.setClientIdentifier( clientId );
  mOauth2Flow.setScope( QString() );

  if ( !mOauth2ReplyHandler )
  {
#ifdef MOBILE_OS
    mOauth2ReplyHandler = new QOAuthUriSchemeReplyHandler( CALLBACK_URL, &mOauth2Flow );
#else
    mOauth2ReplyHandler = new QOAuthHttpServerReplyHandler( OAUTH2_LISTEN_ADDRESS, OAUTH2_LISTEN_PORT, &mOauth2Flow );
    const QString msg = tr( "You can now close this page and return to Mergin Maps" );
    mOauth2ReplyHandler->setCallbackText( msg );
#endif

    mOauth2Flow.setReplyHandler( mOauth2ReplyHandler );

    connect( &mOauth2Flow, &QAbstractOAuth::authorizeWithBrowser, this, [this]( const QUrl & url )
    {
      CoreUtils::log( "SSO", QStringLiteral( "Opening browser to authorize: %1" ).arg( url.toString() ) );
      QDesktopServices::openUrl( url );
    } );

    connect( &mOauth2Flow, &QAbstractOAuth::granted, this, [this]
    {
      CoreUtils::log( "SSO", QStringLiteral( "Successfully authorized, token expires at: %1" ).arg( mOauth2Flow.expirationAt().toString() ) );
      mUserAuth->setFromSso( mOauth2Flow.token(), mOauth2Flow.expirationAt() );
      mOauth2ReplyHandler->close();
      getUserInfo();
    } );

    connect( &mOauth2Flow, &QAbstractOAuth2::error, this, [this]( const QString & error, const QString & errorDescription, const QUrl & uri )
    {
      Q_UNUSED( uri )
      CoreUtils::log( "SSO", QStringLiteral( "OAuth2 error %1: %2" ).arg( error, errorDescription ) );
      mOauth2ReplyHandler->close();
      emit authFailed();
      emit notifyError( tr( "SSO authorization failed" ) );
    } );
  }

  if ( !mOauth2ReplyHandler->isListening() )
  {
#ifdef MOBILE_OS
    mOauth2ReplyHandler->listen();
#else
    mOauth2ReplyHandler->listen( OAUTH2_LISTEN_ADDRESS, OAUTH2_LISTEN_PORT );
#endif
  }

  if ( mOauth2ReplyHandler->isListening() )
  {
    // Initiate the authorization
#ifdef MOBILE_OS
    CoreUtils::log( "SSO", QStringLiteral( "Listening for callback" ) );
#else
    CoreUtils::log( "SSO", QStringLiteral( "Listening for callback on port: %1" ).arg( mOauth2ReplyHandler->port() ) );
#endif
    mOauth2Flow.grant();
  }
  else
  {
    CoreUtils::log( "SSO", QStringLiteral( "Could not start listening for callback" ) );
    emit authFailed();
    emit notifyError( tr( "SSO authorization failed" ) );
  }
}

void MerginApi::abortSsoFlow()
{
  CoreUtils::log( "SSO", QStringLiteral( "SSO flow aborted" ) );
  if ( mOauth2ReplyHandler && mOauth2ReplyHandler->isListening() )
    mOauth2ReplyHandler->close();
}

bool MerginApi::isRetryableNetworkError( const QNetworkReply *reply )
{
  Q_ASSERT( reply );

  const QNetworkReply::NetworkError err = reply->error();

  const bool isRetryableError = ( err == QNetworkReply::TimeoutError ||
                                  err == QNetworkReply::TemporaryNetworkFailureError ||
                                  err == QNetworkReply::NetworkSessionFailedError ||
                                  err == QNetworkReply::UnknownNetworkError ||
                                  err == QNetworkReply::RemoteHostClosedError ||
                                  err == QNetworkReply::ProxyConnectionClosedError ||
                                  err == QNetworkReply::ProxyTimeoutError ||
                                  err == QNetworkReply::UnknownProxyError ||
                                  err == QNetworkReply::ServiceUnavailableError );

  return isRetryableError;
}

void MerginApi::setNetworkManager( QNetworkAccessManager *manager )
{
  if ( !manager )
    return;

  if ( mManager == manager )
    return;

  mManager = manager;

  emit networkManagerChanged();
}

bool MerginApi::apiSupportsSso() const
{
  return mApiSupportsSso;
}

void MerginApi::setApiSupportsSso( const bool ssoSupported )
{
  if ( mApiSupportsSso == ssoSupported )
    return;

  mApiSupportsSso = ssoSupported;
  emit apiSupportsSsoChanged();
}
