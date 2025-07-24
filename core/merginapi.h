/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MERGINAPI_H
#define MERGINAPI_H

#include <memory>

#include <QNetworkReply>
#include <QEventLoop>
#include <QPointer>
#include <QSet>
#include <QByteArray>
#include <utility>
#include <QOAuth2AuthorizationCodeFlow>

#include "merginapistatus.h"
#include "merginservertype.h"
#include "merginerrortypes.h"
#include "merginprojectmetadata.h"
#include "localprojectsmanager.h"
#include "project.h"
#include "merginsubscriptioninfo.h"
#include "merginuserinfo.h"
#include "merginworkspaceinfo.h"
#include "merginuserauth.h"

#ifdef MOBILE_OS
class QOAuthUriSchemeReplyHandler;
#else
class QOAuthHttpServerReplyHandler;
#endif

struct ProjectDiff
{
  // changes that should be pushed
  QSet<QString> localAdded;
  QSet<QString> localUpdated;
  QSet<QString> localDeleted;

  // changes that should be pulled
  QSet<QString> remoteAdded;
  QSet<QString> remoteUpdated;
  QSet<QString> remoteDeleted;

  // to resolve conflict: we make a copy of the file
  QSet<QString> conflictRemoteUpdatedLocalUpdated;
  QSet<QString> conflictRemoteAddedLocalAdded;

  // to resolve conflict: we keep the updated version
  QSet<QString> conflictRemoteDeletedLocalUpdated;
  QSet<QString> conflictRemoteUpdatedLocalDeleted;

  // TODO: non-conflicting changes? R-A/L-A, R-U/L-U, R-D/L-D

  bool operator==( const ProjectDiff &other ) const
  {
    return localAdded == other.localAdded &&
           localUpdated == other.localUpdated &&
           localDeleted == other.localDeleted &&
           remoteAdded == other.remoteAdded &&
           remoteUpdated == other.remoteUpdated &&
           remoteDeleted == other.remoteDeleted &&
           conflictRemoteUpdatedLocalUpdated == other.conflictRemoteUpdatedLocalUpdated &&
           conflictRemoteAddedLocalAdded == other.conflictRemoteAddedLocalAdded &&
           conflictRemoteDeletedLocalUpdated == other.conflictRemoteDeletedLocalUpdated &&
           conflictRemoteUpdatedLocalDeleted == other.conflictRemoteUpdatedLocalDeleted;
  }

  QString dump() const
  {
    QStringList lines;
    lines << "--- project diff ---";
    lines << QString( "local: %1 added, %2 updated, %3 deleted" )
          .arg( localAdded.count() )
          .arg( localUpdated.count() )
          .arg( localDeleted.count() );
    lines << QString( "remote: %1 added, %2 updated, %3 deleted" )
          .arg( remoteAdded.count() )
          .arg( remoteUpdated.count() )
          .arg( remoteDeleted.count() );
    lines << QString( "conflicts: %1 RU-LU, %2 RA-LA, %3 RD-LU, %4 RU-LD" )
          .arg( conflictRemoteUpdatedLocalUpdated.count() )
          .arg( conflictRemoteAddedLocalAdded.count() )
          .arg( conflictRemoteDeletedLocalUpdated.count() )
          .arg( conflictRemoteUpdatedLocalDeleted.count() );
    return lines.join( "\n" );
  }
};


/**
 * A unit of download that should be downloaded during project pull.
 * This is either a chunk of a full file or it is a single diff between two versions.
 */
struct DownloadQueueItem
{
  DownloadQueueItem( QString fp, qint64 s, int v, qint64 rf = -1, qint64 rt = -1, bool diff = false );

  QString filePath;          //!< path within the project
  qint64 size;               //!< size of the item in bytes
  int version = -1;          //!< what version to download  (for ordinary files it will be the target version, for diffs it can be different version)
  qint64 rangeFrom = -1;     //!< what range of bytes to download (-1 if downloading the whole file)
  qint64 rangeTo = -1;       //!< what range of bytes to download (-1 if downloading the whole file)
  bool downloadDiff = false; //!< whether to download just the diff between the previous version and the current one
  QString tempFileName;      //!< relative filename of the temporary file where the downloaded content will be stored
};


/**
 * Entry for each file that will be updated. At the end of a successful pull of new data,
 * all the tasks are executed.
 */
struct PullTask
{
  enum Method
  {
    Copy,           //!< simply write a new version of the file
    CopyConflict,   //!< like Copy, but also create a conflict file of the locally modified file
    ApplyDiff,      //!< apply diffs
    Delete,         //!< remove files that have been removed from the server
  };

  PullTask( const Method m, QString fp, const QList<DownloadQueueItem> &d )
    : method( m ), filePath( std::move( fp ) ), data( d ) {}

  Method method;                  //!< what to do with the file
  QString filePath;               //!< what is the file path within project
  QList<DownloadQueueItem> data;  //!< list of chunks / list of diffs to apply
};

struct TransactionStatus
{
  enum TransactionType
  {
    Push = 0,
    Pull
  };

  qreal totalSize = 0;     //!< total size (in bytes) of files to be pushed or pulled
  qint64 transferedSize = 0;  //!< size (in bytes) of amount of data transferred so far
  QString transactionUUID; //!< only for push. Initially dummy non-empty string, after server confirms a valid UUID, on finish/cancel it is empty

  // pull replies
  QPointer<QNetworkReply> replyPullProjectInfo;
  QPointer<QNetworkReply> replyPullServerConfig;
  QSet<QNetworkReply *> replyPullItems;

  // push replies
  QPointer<QNetworkReply> replyPushProjectInfo;
  QPointer<QNetworkReply> replyPushStart;
  QPointer<QNetworkReply> replyPushFile;
  QPointer<QNetworkReply> replyPushFinish;

  // pull-related data
  QList<DownloadQueueItem> downloadQueue;  //!< pending list of stuff to download - chunks of project files or diff files (at the end of transaction it is empty)
  QList<PullTask> pullTasks;  //!< tasks to do at the end of pull when everything has been downloaded
  bool pullItemsAborting = false;   //!< indicates whether we have started to abort requests in replyPullItems

  // push-related data
  QList<MerginFile> pushQueue; //!< pending list of files to push (at the end of transaction it is empty)
  QList<MerginFile> pushDiffFiles;  //!< these are just diff files for push - we don't remove them when pushing chunks (needed for finalization)

  // retry handling
  int retryCount = 0;  //!< current number of retry attempts for failed network requests
  static constexpr int MAX_RETRY_COUNT = 5;  //!< maximum number of retry attempts for failed network requests

  QString projectDir;
  QByteArray projectMetadata;  //!< metadata of the new project (not parsed)
  bool firstTimeDownload = false;   //!< only for update. whether this is first time to download the project (on failure we would also remove the project folder)
  bool pullBeforePush = false; //!< true when we're first doing update before doing actual upload. Used in sync finalization to figure out whether restart with upload or finish.
  bool isInitialPush = false; //!< true when we are first time uploading the project - migration to Mergin
  bool gpkgSchemaChanged = false; //!< true when GPKG schema changes found

  int version = -1;  //!< version to which we are updating / the version which we have uploaded

  ProjectDiff diff;

  bool configAllowed = false; //!< if true, seeks for mergin-config and alters synchronization process based on it
  MerginConfig config; //!< defines additional behavior of the transaction (e.g. selective sync)

  TransactionType type;
};

typedef QHash<QString, TransactionStatus> Transactions;

Q_DECLARE_METATYPE( Transactions );

class ErrorCode
{
    Q_GADGET
  public:
    enum Value
    {
      Unknown = 0,
      ProjectsLimitHit,
      StorageLimitHit
    };
    Q_ENUM( Value );
};

class MerginApi: public QObject
{
    Q_OBJECT
    Q_PROPERTY( MerginUserAuth *userAuth READ userAuth NOTIFY authChanged )
    Q_PROPERTY( MerginUserInfo *userInfo READ userInfo NOTIFY userInfoChanged )
    Q_PROPERTY( MerginWorkspaceInfo *workspaceInfo READ workspaceInfo NOTIFY workspaceInfoChanged )
    Q_PROPERTY( MerginSubscriptionInfo *subscriptionInfo READ subscriptionInfo NOTIFY subscriptionInfoChanged )
    Q_PROPERTY( QString apiRoot READ apiRoot WRITE setApiRoot NOTIFY apiRootChanged )
    Q_PROPERTY( bool apiSupportsSubscriptions READ apiSupportsSubscriptions NOTIFY apiSupportsSubscriptionsChanged )
    // supportsSelectiveSync if true, fetches mergin-config.json in project and changes sync behavior based on its content (selective sync)
    Q_PROPERTY( bool supportsSelectiveSync READ supportsSelectiveSync NOTIFY supportsSelectiveSyncChanged )
    Q_PROPERTY( /*MerginApiStatus::ApiStatus*/ int apiVersionStatus READ apiVersionStatus NOTIFY apiVersionStatusChanged )
    Q_PROPERTY( /*MerginServerType::ServerType*/ int serverType READ serverType NOTIFY serverTypeChanged )
    Q_PROPERTY( bool apiSupportsWorkspaces READ apiSupportsWorkspaces NOTIFY apiSupportsWorkspacesChanged )
    Q_PROPERTY( bool apiSupportsSso READ apiSupportsSso WRITE setApiSupportsSso NOTIFY apiSupportsSsoChanged )

  public:

    explicit MerginApi( LocalProjectsManager &localProjects, QObject *parent = nullptr );
    ~MerginApi() override = default;

    MerginUserAuth *userAuth() const;
    MerginUserInfo *userInfo() const;
    MerginWorkspaceInfo *workspaceInfo() const;
    MerginSubscriptionInfo *subscriptionInfo() const;

    /**
     * Returns path of the local directory in which all projects are stored.
     * Each project is one subdirectory.
     * \note returns the directory without a trailing slash
     */
    QString projectsPath() const { return mDataDir; }

    //! Returns reference to the cache of local projects
    LocalProjectsManager &localProjectsManager() const { return mLocalProjects; }

    /**
     * Sends non-blocking GET request to the server to listProjects. On listProjectsReplyFinished,
     * when a response is received, parses projects json and sets mMerginProjects. The authorization is not required
     * for "exploring" all public projects. However, it can be applied to fetch more results.
     * Eventually emits listProjectsFinished on which ProjectPanel (qml component) updates content.
     * \param searchExpression Search filter on projects name.
     * \param flag If defined, it is used to filter out projects tagged as 'created' or 'shared' with an authorized user
     * \param page Requested page of projects.
     * \returns unique id of a request
     */
    QString listProjects( const QString &searchExpression = QString(),
                          const QString &flag = QString(),
                          int page = 1 );

    /**
     * Sends non-blocking GET request to the server to listProjectsByName API. Response is handled in
     * listProjectsByNameFinished method. Projects are parsed from response JSON.
     *
     * \param projectNames QStringList of project full names (namespace/name)
     * \returns unique id of sent request
     */
    QString listProjectsByName( const QStringList &projectNames = QStringList() );

    /**
     * Fallback method for listProjectsByName, which tries to request projects by IDs instead of names.
     * Uses getProjectsDetails internally.
     * \param projectIds QStringList of project IDs
     * \see listProjectsByName
     */
    void refetchBrokenProjects( const QStringList &projectIds );

    /**
     * Sends non-blocking POST request to the server to pull (download) a project with a given name.
     * On pullProjectReplyFinished, when a response is received, parses data-stream to files and rewrites local files
     * with them. Extra files which don't match server files are removed. Emits syncProjectFinished at the end.
     * If update has been successful, updates metadata file of the project.
     * \param projectFullName Project's "namespace/name" full name used for requests
     * \param projectId Project's ID used for internal processing
     * \param withAuth If True, request is constructed with current authorization
     * \return true when sync has started, false otherwise (e.g. due to a missing authorization or invalid server)
     */
    bool pullProject( const QString &projectFullName, const QString &projectId, bool withAuth = true );

    /**
     * Sends non-blocking POST request to the server to push changes in a project with a given name.
     * At the beginning it checks if there are any changes on server and pulls them if so. If the pull was successful,
     * it sends post request with list of local changes and modified/newly added files in JSON.
     * Emits syncProjectFinished at the end.
     * \param projectFullName Project's "namespace/name" full name used for requests
     * \param projectId Project's ID used for internal processing
     * \param isInitialPush indicates if this is first push of the project (project creation)
     * \return true when sync has started, false otherwise (e.g. due to a missing authorization or invalid server)
     */
    bool pushProject( const QString &projectFullName, const QString &projectId, bool isInitialPush = false );

    /**
     * Sends non-blocking POST request to the server to cancel a running push of a project with a given name.
     * If push has not started yet and a client is waiting for transaction UUID, it cancels the procedure just
     * on client side without sending cancel request to the server.
     * \param projectId ID of project to cancel push for
     * \note pushCanceled() signal is emitted when the reply to the cancel request is received
     */
    void cancelPush( const QString &projectId );

    /**
     * Cancels pull either (1) before project data download starts or
     * (2) when data transfer has begun - connections are aborted.
     * \param projectId Project's ID to cancel
     */
    void cancelPull( const QString &projectId );

    /**
    * Attempts to authorize user with the login and password
    */
    Q_INVOKABLE void authorize( const QString &login, const QString &password );

    /**
     * Requests the server's sso config
     * If server config is single tenant, the sso flow is started
     *
     * \see startSsoFlow()
     */
    Q_INVOKABLE void requestSsoConfig();

    /**
     * Requests the available sso connections for the specified email
     * If a connection is found, sso flow is started
     *
     * \see ssoConnectionsReplyFinished(), startSsoFlow()
     */
    Q_INVOKABLE void requestSsoConnections( const QString &email );

    //! Stops the OAuth2 reply handlers from listening
    Q_INVOKABLE void abortSsoFlow();

    Q_INVOKABLE void getUserInfo();
    Q_INVOKABLE void getWorkspaceInfo();
    Q_INVOKABLE void getServiceInfo();
    void clearAuth();
    Q_INVOKABLE QString resetPasswordUrl() const;

    /**
    * Registers new user.
    * \param email Email to associate with the new account
    * \param password Password to associate with the new account
    * \param acceptedTOC Whether user accepted Terms and Conditions
    */
    Q_INVOKABLE void registerUser(
      const QString &email,
      const QString &password,
      bool acceptedTOC
    );

    /**
    * Store extra marketing information for new user that created workspace
    * \param marketingChannel How you found us?
    * \param industry Which industry?
    * \param wantsNewsletter Whether user wants to receive newsletter
    */
    Q_INVOKABLE void postRegisterUser(
      const QString &marketingChannel,
      const QString &industry,
      bool wantsNewsletter
    );

    /**
    * Pings Mergin server and checks its version with required version defined in version.pri
    * Accordingly sets mApiVersionStatus variable (reset when mergin url is changed).
    * The function is skipped if version has been checked and passed.
    */
    Q_INVOKABLE void pingMergin();

    /**
    * Makes a mergin project to be local by removing .mergin folder. Updates project's info and related models accordingly.
    * \param projectId Project id of project that will be detached from Mergin
    * \param informUser Whether we notify user with notification about success
    */
    void detachProjectFromMergin( const QString &projectId, bool informUser = true );

    /**
    * Deletes all local projects and then tries to remove user account.
    */
    Q_INVOKABLE void deleteAccount();

    static constexpr int MERGIN_API_VERSION_MAJOR = 2020;
    static constexpr int MERGIN_API_VERSION_MINOR = 4;
    static constexpr int MINIMUM_SERVER_VERSION_MAJOR = 2023;
    static constexpr int MINIMUM_SERVER_VERSION_MINOR = 2;
    static const QString sMetadataFile;
    static const QString sMetadataFolder;
    static const QString sMerginConfigFile;
    static const QString sDefaultApiRoot;
    static const QString sSyncCanceledMessage;

    static QString defaultApiRoot() { return sDefaultApiRoot; }

    static bool isFileDiffable( const QString &fileName ) { return fileName.endsWith( ".gpkg" ); }

    static ProjectDiff localProjectChanges( const QString &projectDir );
    static bool hasLocalProjectChanges( const QString &projectDir, bool supportsSelectiveSync );

    /**
     * Parse major and minor version number from version string
     * \param version full server version string
     * \param major parsed major number
     * \param minor parsed minor number
     * @return true when parsing was successful
     */
    static bool parseVersion( const QString &version, int &major, int &minor );

    /**
    * Creates an empty project on Mergin server. isPublic determines if the new project will be visible to all or private
    * \return true when project creation has started, false otherwise (e.g. due to a missing authorization)
    */
    bool createProject( const QString &projectNamespace, const QString &projectName, const QString &projectId, bool isPublic = false );

    // Test functions
    /**
    * Deletes the project of given projectId on Mergin server.
    * \note This deletion is immediately done, if you want to schedule project for deletion use
    * \code /v2/projects/{id}/scheduleDelete \endcode
    */
    void deleteProject( const QString &projectId, bool informUser = true );

    LocalProject getLocalProject( const QString &projectId ) const;

    // Production and Test functions (therefore not private)

    /**
     * Compares project files from three sources:
     * - "old" server version (what was downloaded from server) - read from the project directory's stored metadata
     * - "new" server version (what is currently available on server) - newly fetched from the server
     * - local file version (what is currently in the project directory) - created on the fly from the local directory content
     *
     * The function assigns each of the files to the kind of change that happened to it.
     * Files that have not been changed are not present in the final diff.
     *
     * Optionally, it is possible to set "allowConfig" to true, which enables to alter synchronization logic. Selective sync
     * is one option, it ignores changes (adding / removing) to files in specified folder. These files are only uploaded to server.
     * "lastSyncConfig" represents config used during last synchronization. It is used to find files that needs to be downloaded
     *  after changes in "config"
     *
     * Without the three sources it is possible to miss some of the updates that need to be handled (e.g. conflicts)
     */
    static ProjectDiff compareProjectFiles(
      const QList<MerginFile> &oldServerFiles,
      const QList<MerginFile> &newServerFiles,
      const QList<MerginFile> &localFiles,
      const QString &projectDir,
      bool allowConfig = false,
      const MerginConfig &config = MerginConfig(),
      const MerginConfig &lastSyncConfig = MerginConfig()
    );

    /**
     * Finds if project files from two sources are same
     * - "old" server version (what was downloaded from server) - read from the project directory's stored metadata
     * - local file version (what is currently in the project directory) - created on the fly from the local directory content
     *
     * The function returns true if:
     *   - there is any local file not present in "old" server version files
     *   - there is any local file missing in "old" server version files
     *   - there is different checksum of any non-diffable file (e.g. CSV file)
     *   - there is different content of any diffable file (e.g. GeoPackage)
     */
    static bool hasLocalChanges(
      const QList<MerginFile> &oldServerFiles,
      const QList<MerginFile> &localFiles,
      const QString &projectDir,
      const MerginConfig &config
    );

    static QList<MerginFile> getLocalProjectFiles( const QString &projectPath );

    QString apiRoot() const;
    void setApiRoot( const QString &apiRoot );

    MerginApiStatus::VersionStatus apiVersionStatus() const;
    void setApiVersionStatus( const MerginApiStatus::VersionStatus &apiVersionStatus );

    //! Returns details about currently active transactions (both push and pull). Useful for tests
    Transactions transactions() const { return mTransactionalStatus; }

    // Returns true for files that are under .mergin folder or contains ignored extension from sIgnoreExtensions
    static bool isInIgnore( const QFileInfo &info );

    /**
     * Performs checks and returns if a given file is excluded from the sync.
     * If selective-sync-enabled is true, it checks if a file extension is from excludeSync extension list.
     * If selective-sync-dir is defined, additionally checks, if the file is located in selective-sync-dir or in its
     * subdir, otherwise a project dir is considered as selective-sync-dir and therefore the path check is redundant
     * (since given filePath is relative to the project dir.).
     * @param filePath Relative path of a file to project directory.
     * @param config MerginConfig parsed from JSON, selective-sync properties are read from it.
     * @return True, if a file at given filePath suppose to be excluded from sync.
     */
    static bool excludeFromSync( const QString &filePath, const MerginConfig &config );

    bool apiSupportsSubscriptions() const;
    void setApiSupportsSubscriptions( bool apiSupportsSubscriptions );

    bool supportsSelectiveSync() const;
    void setSupportsSelectiveSync( bool supportsSelectiveSync );

    /**
     * Determine Mergin server type by querying /config endpoint.
     * Possible types are: saas, ce, ee and legacy
     */
    void getServerConfig();

    MerginServerType::ServerType serverType() const;
    void setServerType( const MerginServerType::ServerType &serverType );

    /**
     * Reads server details and user details from QSettings.
     */
    void loadCache();

    /**
     * Sends non-blocking GET request to the server to list user workspaces.
     * On listWorkspacesReplyFinished, when a response is received, parses
     * workspaces json and emits signal with a QMap<int, QString> containing
     * workspace id and name.
     */
    void listWorkspaces();

    /**
     * Sends non-blocking GET request to the server to list available
     * invitations. On listInvitationsReplyFinished, when a response is
     * received, parses invitations json and emits signal with a
     * QMap<QString, QString> containing invitation uuid and name.
     */
    void listInvitations();

    /**
    * Accepts or discards an invitation to join workspace.
    * \param uuid Invitation UUID
    * \param accept Whether user accepted invitation
    */
    Q_INVOKABLE void processInvitation( const QString &uuid, bool accept );

    /**
    * Creates a new workspace on Mergin server.
    * \param workspaceName
    */
    Q_INVOKABLE bool createWorkspace( const QString &workspaceName );

    /**
     * Clears authorisation data
     */
    Q_INVOKABLE void signOut();

    /**
     * Emits API calls that bear user information like username, workspaces and service
     */
    Q_INVOKABLE void refreshUserData();

    /**
     * Returns true if server supports workspaces
     */
    bool apiSupportsWorkspaces() const;

    /**
     * Returns true if the configured server has SSO enabled
     */
    bool apiSupportsSso() const;

    /**
     * Reloads project metadata role by fetching latest information from server.
     */
    void reloadProjectRole( const QString &projectId );

    /**
     * Returns the network manager used for Mergin API requests
     */
    QNetworkAccessManager *networkManager() const { return mManager; }

    /**
     * Sets the network manager to be used for Mergin API requests
     * Function will return early if manager is null.
     */
    void setNetworkManager( QNetworkAccessManager *manager );

    /**
     * Makes this API available to use/not to use SSO
     */
    void setApiSupportsSso( bool ssoSupported );

  signals:
    void apiSupportsSubscriptionsChanged();
    void supportsSelectiveSyncChanged();

    void listProjectsFinished( const MerginProjectsList &merginProjects, int projectCount, int page, QString requestId );
    void listProjectsFailed();
    void listProjectsByNameFinished( const MerginProjectsList &merginProjects, QString requestId );
    void syncProjectFinished( const QString &projectId, bool successfully, int version );
    void projectReloadNeededAfterSync( const QString &projectId );
    /**
     * Emitted when sync starts/finishes or the progress changes - useful to give a clue in the GUI about the status.
     * Normally progress is in interval [0, 1] as data get pushed or pulled.
     * With no pending sync, progress is set to -1
     */
    void syncProjectStatusChanged( const QString &projectId, qreal progress );

    void networkErrorOccurred(
      const QString &message,
      int httpCode = -1,
      const QString &projectId = QString()
    );

    void storageLimitReached( qreal uploadSize );
    void projectLimitReached( int maxProjects, const QString &message );
    void projectCreationFailed();
    void migrationRequested( const QString &version );
    void notifySuccess( const QString &message );
    void notifyInfo( const QString &message );
    void notifyError( const QString &message );
    void authRequested();
    void ssoLoginExpired();
    void authChanged();
    void authFailed();
    void registrationSucceeded();
    void registrationFailed( const QString &msg, RegistrationError::RegistrationErrorType type = RegistrationError::RegistrationErrorType::OTHER );
    void postRegistrationSucceeded();
    void postRegistrationFailed( const QString &msg );
    void apiRootChanged();
    void apiVersionStatusChanged();
    void projectCreated( const QString &projectId, bool result );
    void userInfoChanged();
    void workspaceInfoChanged();
    void subscriptionInfoChanged();
    void activeWorkspaceChanged();
    void configChanged();
    void pingMerginFinished( const QString &apiVersion, bool serverSupportsSubscriptions, const QString &msg );
    void pullFilesStarted();
    void pushFilesStarted();
    void pushCanceled( const QString &projectId );
    void projectDataChanged( const QString &projectFullName, const QString &projectId );
    void projectDetached( const QString &projectId );
    void projectAttachedToMergin( const QString &projectId );
    void refetchBrokenProjectsFinished( const MerginProjectsList &projectList );

    void projectAlreadyOnLatestVersion( const QString &projectId );
    void missingAuthorizationError( const QString &projectId );
    void accountDeleted( bool result );
    void userIsAnOrgOwnerError();

    void serverTypeChanged();

    void listWorkspacesFailed();
    void listWorkspacesFinished( const QMap<int, QString> &workspaces );

    void listInvitationsFailed();
    void listInvitationsFinished( const QList<MerginInvitation> &invitations );

    void processInvitationFailed();
    void processInvitationFinished( bool accepted );

    void workspaceCreated( const QString &workspaceName );
    void userInfoReplyFinished();
    void getWorkspaceInfoFinished();

    void hasWorkspacesChanged();
    void apiSupportsWorkspacesChanged();

    void serverWasUpgraded();

    void projectRoleUpdated( const QString &projectId, const QString &role );

    void networkManagerChanged();

    void downloadItemRetried( const QString &projectId, int retryCount );

    void projectIdChanged( const QString &projectFullName, QString projectId );

    void apiSupportsSsoChanged();

    //! Emitted when server sso config is returned and server is multi tenant
    void ssoConfigIsMultiTenant();

  private slots:
    void listProjectsReplyFinished( const QString &requestId );
    void listProjectsByNameReplyFinished( const QString &requestId );
    void getProjectsDetailsReplyFinished();
    void refetchBrokenProjectsReplyFinished();

    // Pull slots
    void pullInfoReplyFinished();
    void downloadItemReplyFinished( const DownloadQueueItem &item );
    void cacheServerConfig();

    // Push slots
    void pushStartReplyFinished();
    void pushInfoReplyFinished();
    void pushFileReplyFinished();
    void pushFinishReplyFinished();
    void pushCancelReplyFinished();

    void getUserInfoFinished();
    void getWorkspaceInfoReplyFinished();
    void getServiceInfoReplyFinished();
    void saveAuthData() const;
    void createProjectFinished();
    void deleteProjectFinished( bool informUser = true );
    void authorizeFinished();
    void registrationFinished( const QString &login = QString(), const QString &password = QString() );
    void postRegistrationFinished();
    void pingMerginReplyFinished();
    void deleteAccountFinished();

    /**
     * @brief When plan has been changed, an extra userInfo request is needed to update also storage.
     * Calls user info only when has authData, otherwise slots catches the signal from clearing user data after signing out.
     */
    void onPlanProductIdChanged();

    void getServerConfigReplyFinished();
    void listWorkspacesReplyFinished();
    void listInvitationsReplyFinished();
    void processInvitationReplyFinished();
    void createWorkspaceReplyFinished();

    void ssoConfigReplyFinished();
    void ssoConnectionsReplyFinished();

  private:
    MerginProject parseProjectMetadata( const QJsonObject &project );
    MerginProjectsList parseProjectsFromJson( const QJsonDocument &object );
    static QStringList generateChunkIdsForSize( qint64 fileSize );
    QJsonArray prepareUploadChangesJSON( const QList<MerginFile> &files );
    static QString getApiKey( const QString &serverName );
    void abortPullItems( const QString &projectId );

    /**
     * Sends non-blocking POST request to the server to upload a file (chunk).
     * \param projectFullName Namespace/name
     * \param projectId ID of project
     * \param json project info containing metadata for upload
     */
    void pushStart( const QString &projectFullName, const QString &projectId, const QByteArray &json );

    /**
     * Sends non-blocking POST request to the server to upload a file (chunk).
     * \param projectFullName Namespace/name
     * \param projectId ID of project
     * \param transactionUUID Transaction ID which servers sends on uploadStart
     * \param file Mergin file to upload
     * \param chunkNo Chunk number of given file to be uploaded
     */
    void pushFile( const QString &projectFullName, const QString &projectId, const QString &transactionUUID, const MerginFile &file, int chunkNo = 0 );

    /**
     * Closing request after successful push.
     * \param projectFullName Namespace/name
     * \param projectId ID of project
     * \param transactionUUID transaction UUID to match upload process on the server
     */
    void pushFinish( const QString &projectFullName, const QString &projectId, const QString &transactionUUID );
    void sendPushCancelRequest( const QString &projectFullName, const QString &projectId, const QString &transactionUUID );

    bool writeData( const QByteArray &data, const QString &path );
    void createPathIfNotExists( const QString &filePath );

    static QSet<QString> listFiles( const QString &projectPath );

    bool validateAuth();
    void checkMerginVersion( const QString &apiVersion, bool serverSupportsSubscriptions, const QString &msg = QStringLiteral() );

    /**
    * Extracts string code of an error json. If it's not json or value cannot be parsed, QString() is return;
    * \param data Data received from mergin server on a request failed.
    */
    QString extractServerErrorCode( const QByteArray &data );
    /**
    * Extracts value of an error json. If it's not json or value cannot be parsed, QVariant() is return;
    * \param data Data received from mergin server on a request failed.
    * \param key Where should be a value from data
    */
    QVariant extractServerErrorValue( const QByteArray &data, const QString &key );
    /**
    * Extracts detail (message) of an error json. If it's not json or detail cannot be parsed, the whole data are return;
    * \param data Data received from mergin server on a request failed.
    */
    QString extractServerErrorMsg( const QByteArray &data );
    /**
    * Returns a temporary project path.
    * \param projectFullName
    */
    QString getTempProjectDir( const QString &projectFullName ) const;

    /**
     * Creates a request to get project details with list of project files. The reason why we use both variants is
     * the limitation of API, currently the endpoint which uses project ID doesn't return necessary versioning
     * information for synchronization workflow. Thus, for synchronization use this method and for basic project details
     * use \a getProjectDetails.
     * \param projectFullName Project full name to use for request
     * \param projectId project ID
     * \param withAuth Specifies if the request will have authentication token
     * \see getProjectDetails, getProjectsDetails
     */
    QNetworkReply *getProjectInfo( const QString &projectFullName, const QString &projectId, bool withAuth = true );

    /**
     * Creates a request to get project details. The reason why we use both variants is
     * the limitation of API, currently the endpoint which uses project ID doesn't return necessary versioning
     * information for synchronization workflow. Thus, for synchronization use \a getProjectInfo.
     * \param projectId project ID to use for request
     * \param withAuth Specifies if the request will have authentication token
     * \see getProjectInfo, getProjectsDetails
     */
    QNetworkReply *getProjectDetails( const QString &projectId, bool withAuth = true );

    /**
     * Similar to getProjectDetails, but it can fetch multiple projects in one call. Also, we use it as a fallback for
     * getProjectInfo during syncing. The reason we use this instead of getProjectDetails is that this response is
     * lighter and doesn't include file history.
     * \param projectIds project IDs to use for request
     * \param withAuth Specifies if the request will have authentication token
     * \see getProjectInfo, getProjectDetails
     */
    QNetworkReply *getProjectsDetails( const QStringList &projectIds, bool withAuth = true );

    //! Called when pull of project data has finished to finalize things and emit sync finished signal
    void finalizeProjectPull( const QString &projectId );
    void finalizeProjectPullCopy( const QString &projectFullName, const QString &projectDir, const QString &tempDir, const QString &filePath, const QList<DownloadQueueItem> &items );
    bool finalizeProjectPullApplyDiff( const QString &projectFullName, const QString &projectId, const QString &projectDir, const QString &tempDir, const QString &filePath, const QList<DownloadQueueItem> &items );

    //! Takes care of removal of the transaction, writing new metadata and emits syncProjectFinished()
    void finishProjectSync( const QString &projectFullName, const QString &projectId, bool syncSuccessful );
    void prepareProjectPull( const QString &projectId, const QByteArray &data );
    void startProjectPull( const QString &projectId );

    //! Takes care of finding the correct config file, appends it to current transaction and proceeds with project pull
    void prepareDownloadConfig( const QString &projectId, bool downloaded = false );
    void requestServerConfig( const QString &projectId );

    //! Starts download request of another item
    void downloadNextItem( const QString &projectId );

    //! Removes temp folder for project
    void removeProjectsTempFolder( const QString &projectNamespace, const QString &projectName ) const;

    //! Refreshes auth token if it is expired. It does a blocking call to authorize.
    //! Works only when login, password and token is set in UserAuth. Does nothing if using SSO.
    void refreshAuthToken();

    /**
     * Checks if a network error should trigger a retry attempt.
     * \param reply Network reply to check for retryable errors
     * \returns True if the error should trigger a retry, false otherwise
     */
    bool isRetryableNetworkError( const QNetworkReply *reply );

    QNetworkRequest getDefaultRequest( bool withAuth = true ) const;

    bool projectFileHasBeenUpdated( const ProjectDiff &diff );

    //! Checks if retrieving the project role from the server was successful and
    //! if it differs from the current project role, emits a signal with new project role
    void reloadProjectRoleReplyFinished();

    //! Updates project role in metadata file
    bool updateCachedProjectRole( const QString &projectId, const QString &newRole ) const;

    //! Retrieves cached role from metadata file
    QString getCachedProjectRole( const QString &projectId ) const;

    void startSsoFlow( const QString &clientId );

    QNetworkAccessManager *mManager = nullptr;

    QString mApiRoot;
    LocalProjectsManager &mLocalProjects;
    QString mDataDir; // dir with all projects

    MerginUserInfo *mUserInfo; //owned by this (qml grouped-properties)
    MerginWorkspaceInfo *mWorkspaceInfo; //owned by this (qml grouped-properties)
    MerginSubscriptionInfo *mSubscriptionInfo; //owned by this (qml grouped-properties)
    MerginUserAuth *mUserAuth; //owned by this (qml grouped-properties)

    enum CustomAttribute
    {
      AttrProjectFullName = QNetworkRequest::User,
      AttrTempFileName    = QNetworkRequest::User + 1,
      AttrWorkspaceName   = QNetworkRequest::User + 2,
      AttrAcceptFlag      = QNetworkRequest::User + 3,
      AttrProjectId       = QNetworkRequest::User + 4,
      AttrAuthUsed        = QNetworkRequest::User + 5
    };

    Transactions mTransactionalStatus; //projectId -> transactionStatus
    static const QSet<QString> sIgnoreExtensions;
    static const QSet<QString> sIgnoreImageExtensions;
    static const QSet<QString> sIgnoreFiles;
    QEventLoop mAuthLoopEvent;
    MerginApiStatus::VersionStatus mApiVersionStatus = MerginApiStatus::VersionStatus::UNKNOWN;
    bool mApiSupportsSubscriptions = false;
    bool mSupportsSelectiveSync = true;
    bool mApiSupportsSso = false;

    static const int UPLOAD_CHUNK_SIZE;
    const int PROJECT_PER_PAGE = 50;
    const QString TEMP_FOLDER = QStringLiteral( ".temp/" );

    static QList<DownloadQueueItem> itemsForFileChunks( const MerginFile &file, int version );
    static QList<DownloadQueueItem> itemsForFileDiffs( const MerginFile &file );

    MerginServerType::ServerType mServerType = MerginServerType::ServerType::OLD;

    QOAuth2AuthorizationCodeFlow mOauth2Flow;
#ifdef MOBILE_OS
    QOAuthUriSchemeReplyHandler *mOauth2ReplyHandler = nullptr; // parented by mOauth2Flow
    static const QString CALLBACK_URL;
#else
    QOAuthHttpServerReplyHandler *mOauth2ReplyHandler = nullptr; // parented by mOauth2Flow
    static const QHostAddress OAUTH2_LISTEN_ADDRESS;
    static const int OAUTH2_LISTEN_PORT;
#endif

    friend class TestMerginApi;
};

#endif // MERGINAPI_H
