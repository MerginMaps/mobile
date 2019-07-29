#ifndef MERGINAPI_H
#define MERGINAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <memory>
#include <QFile>
#include <QFileInfo>

#include "merginapistatus.h"

enum ProjectStatus
{
  NoVersion,  //!< the project is not available locally
  UpToDate,   //!< both server and local copy are in sync with no extra modifications
  OutOfDate,  //!< server has newer version than what is available locally (but the project is not modified locally)
  Modified    //!< there are some local modifications in the project that need to be pushed (note: also server may have newer version)
};
Q_ENUMS( ProjectStatus )

struct MerginFile
{
  QString path;
  QString checksum;
  qint64 size;
  QDateTime mtime;
  QStringList chunks; // used only for upload otherwise suppose to be empty
};

#include <QPointer>


struct ProjectDiff2
{
  // changes that should be pushed (uploaded)
  QSet<QString> localAdded;
  QSet<QString> localUpdated;
  QSet<QString> localDeleted;

  // changes that should be pulled (downloaded)
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


struct TransactionStatus
{
  qreal totalSize = 0;     //!< total size (in bytes) of files to be uploaded or downloaded
  int transferedSize = 0;  //!< size (in bytes) of amount of data transferred so far
  QString transactionUUID; //!< only for upload. Initially dummy non-empty string, after server confirms a valid UUID, on finish/cancel it is empty

  // download replies
  QPointer<QNetworkReply> replyProjectInfo;
  QPointer<QNetworkReply> replyDownloadFile;

  // upload replies
  QPointer<QNetworkReply> replyUploadProjectInfo;
  QPointer<QNetworkReply> replyUploadStart;
  QPointer<QNetworkReply> replyUploadFile;
  QPointer<QNetworkReply> replyUploadFinish;

  QList<MerginFile> files; // either to upload or download

  ProjectDiff2 diff;
};

struct MerginProject
{
  QString name;
  QString projectNamespace;
  QString projectDir;  // full path to the project directory
  QStringList tags;
  QList<MerginFile> files;
  QString version;
  QDateTime clientUpdated; // client's version of project files
  QDateTime serverUpdated; // available latest version of project files on server
  QDateTime lastSyncClient; // local datetime of download/upload/update project
  bool pending = false; // if there is a pending request for downlaod/update a project
  ProjectStatus status = NoVersion;
  int filesCount = -1;  // it's here in addition to "files" because project list only contains this information
  qreal progress = 0;  // progress in case of pending download/upload (values [0..1])
  int creator; // server-side user ID of the project owner (creator)
  QList<int> writers; // server-side user IDs of users having write access to the project

  MerginFile fileInfo( const QString &filePath ) const
  {
    for ( const MerginFile &merginFile : files )
    {
      if ( merginFile.path == filePath )
        return merginFile;
    }
    qDebug() << "requested fileInfo() for non-existant file! " << filePath;
    return MerginFile();
  }
};

struct ProjectDiff
{
  QList<MerginFile> added;
  QList<MerginFile> modified;
  QList<MerginFile> removed;
};


typedef QList<std::shared_ptr<MerginProject>> ProjectList;

class MerginApi: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString username READ username NOTIFY authChanged )
    Q_PROPERTY( int userId READ userId NOTIFY authChanged )
    Q_PROPERTY( int storageLimit READ storageLimit NOTIFY userInfoChanged )
    Q_PROPERTY( int diskUsage READ diskUsage NOTIFY userInfoChanged )
    Q_PROPERTY( QString apiRoot READ apiRoot WRITE setApiRoot NOTIFY apiRootChanged )
    Q_PROPERTY( int apiVersionStatus READ apiVersionStatus NOTIFY apiVersionStatusChanged )
  public:
    explicit MerginApi( const QString &dataDir, QObject *parent = nullptr );
    ~MerginApi() = default;

    /**
     * Returns path of the local directory in which all projects are stored.
     * Each project is one sub-directory.
     * \note returns the directory with a trailing slash
     */
    QString projectsPath() const { return mDataDir; }

    /**
     * Sends non-blocking GET request to the server to listProjects. On listProjectsReplyFinished,
     * when a response is received, parses projects json and sets mMerginProjects.
     * Eventually emits listProjectsFinished on which ProjectPanel (qml component) updates content.
     * \param searchExpression Search filter on projects name.
     * \param user Mergin username used with flag
     * \param flag If defined, it is used to filter out projects tagged as 'created' or 'shared' with given username
     * \param withFilter If true, applies "input" tag in request.
     */
    Q_INVOKABLE void listProjects( const QString &searchExpression = QStringLiteral(), const QString &user = QStringLiteral(),
                                   const QString &flag = QStringLiteral(), const QString &filterTag = QStringLiteral( "input_use" ) );

    /**
     * Sends non-blocking POST request to the server to download/update a project with a given name. On downloadProjectReplyFinished,
     * when a response is received, parses data-stream to files and rewrites local files with them. Extra files which don't match server
     * files are removed. Eventually emits syncProjectFinished on which MerginProjectModel updates status of the project item.
     * If update has been successful, updates metadata file of the project.
     * Emits also notify signal with a message for the GUI.
     * \param projectNamespace Project's namespace used in request.
     * \param projectName  Project's name used in request.
     */
    Q_INVOKABLE void updateProject( const QString &projectNamespace, const QString &projectName );

    /**
     * Sends non-blocking POST request to the server to upload changes in a project with a given name.
     * Firstly updateProject is triggered to fetch new changes. If it was successful, sends update post request with list of local changes
     * and modified/newly added files in JSON. Eventually emits syncProjectFinished on which MerginProjectModel updates status of the project item.
     * Emits also notify signal with a message for the GUI.
     * \param projectNamespace Project's namespace used in request.
     * \param projectName  Project's name used in request.
     */
    Q_INVOKABLE void uploadProject( const QString &projectNamespace, const QString &projectName );

    /**
     * Sends non-blocking POST request to the server to cancel uploading of a project with a given name.
     * If upload has not started yet and a client is waiting for transaction UUID, it cancels the procedure just on client side
     * without sending cancel request to the server.
     * \param projectFullName Project's full name to cancel its upload
     */
    Q_INVOKABLE void uploadCancel( const QString &projectFullName );

    /**
     * To cancel update (1) either before downloading data started - all data related to the download are cleaned;
     * or (2) either when data transfer has begun - connections are aborted followed by calling function once again
     * to clean related data as in the case 1.;
     * \param projectFullName Project's full name to cancel its update
     */
    Q_INVOKABLE void updateCancel( const QString &projectFullName );

    /**
    * Currently no auth service is used, only "username:password" is encoded and asign to mToken.
    * \param username Login user name to Mergin
    * \param password Password to given username to log in to Mergin
    */
    Q_INVOKABLE void authorize( const QString &username, const QString &password );
    Q_INVOKABLE void getUserInfo( const QString &username );
    Q_INVOKABLE void clearAuth();
    Q_INVOKABLE void resetApiRoot();
    Q_INVOKABLE bool hasAuthData();
    /**
    * Pings Mergin server and checks its version with required version defined in version.pri
    * Accordingly sets mApiVersionStatus variable (reset when mergin url is changed).
    * The function is skipped if version has been checked and passed.
    */
    Q_INVOKABLE void pingMergin();

    /**
    * Finds project in merginProjects list according its full name.
    * \param projecFulltName QString to be set to name of a project
    */
    std::shared_ptr<MerginProject> getProject( const QString &projectFullName );

    static const int MERGIN_API_VERSION_MAJOR = 0;
    static const int MERGIN_API_VERSION_MINOR = 4;
    static const QString sMetadataFile;

    static QString defaultApiRoot() { return "https://public.cloudmergin.com/"; }

    /**
    * Finds project in merginProjects list according its full name.
    * \param projectPath Full path to project's folder
    * \param metadataFile Relative path of metafile to project's folder
    */
    static QString getFullProjectName( QString projectNamespace, QString projectName );
    static std::shared_ptr<MerginProject> readProjectMetadataFromPath( const QString &projectPath,
        const QString &metadataFile = MerginApi::sMetadataFile );

    // Test functions
    /**
    * Creates an empty project on Mergin server.
    * \param projectNamespace
    * \param projectName
    */
    void createProject( const QString &projectNamespace, const QString &projectName );
    /**
    * Deletes the project of given namespace and name on Mergin server.
    * \param projectNamespace
    * \param projectName
    */
    void deleteProject( const QString &projectNamespace, const QString &projectName );
    /**
    * Adds a project to list of merginProjects
    * \param projectNamespace
    * \param projectName
    */
    void addProject( std::shared_ptr<MerginProject> project );
    void clearTokenData();

    // Production and Test functions (therefore not private)
    /**
    * Compares files from newFiles with files from current. For instance, of there is an extra file in new,
    * it suppose to appear in 'added'. If there is a missing file in new, it suppose to appear in removed.
    * \param newFiles List of MerginFiles which are compared with current files.
    * \param currentFiles List of MerginFiles which are taken as base in a comparison.
    */
    ProjectDiff compareProjectFiles( const QList<MerginFile> &newFiles, const QList<MerginFile> &currentFiles );

    static ProjectDiff2 compareProjectFiles2( const QList<MerginFile> &oldServerFiles, const QList<MerginFile> &newServerFiles, const QList<MerginFile> &localFiles );

    ProjectList projects();
    QList<MerginFile> getLocalProjectFiles( const QString &projectPath );
    /**
    * Clears projects metadata.
    * \param project std::shared_ptr<MerginProject> to access certain project.
    */
    void clearProject( std::shared_ptr<MerginProject> project );

    QString username() const;

    QString apiRoot() const;
    void setApiRoot( const QString &apiRoot );

    //! Disk usage of current logged in user in Mergin instance in Bytes
    int diskUsage() const;

    //! Total storage limit of current logged in user in Mergin instance in Bytes
    int storageLimit() const;

    int userId() const;
    void setUserId( int userId );

    MerginApiStatus::VersionStatus apiVersionStatus() const;
    void setApiVersionStatus( const MerginApiStatus::VersionStatus &apiVersionStatus );

    //! Returns details about currently active transactions (both download and upload). Useful for tests
    QHash<QString, TransactionStatus> transactions() const { return mTransactionalStatus; }

  signals:
    void listProjectsFinished( const ProjectList &merginProjects );
    void listProjectsFailed();
    void syncProjectFinished( const QString &projectDir, const QString &projectFullName, bool successfully = true );
    void syncProgressUpdated( const QString &projectFullName, qreal progress );
    void reloadProject( const QString &projectDir );
    void networkErrorOccurred( const QString &message, const QString &additionalInfo, bool showAsDialog = false );
    void notify( const QString &message );
    void merginProjectsChanged();
    void authRequested();
    void authChanged();
    void authFailed();
    void apiRootChanged();
    void apiVersionStatusChanged();
    void projectCreated( const QString &projectName );
    void serverProjectDeleted( const QString &projecFullName );
    void userInfoChanged();
    void pingMerginFinished( const QString &apiVersion, const QString &msg );
    void pullFilesStarted();
    //! Emitted when started to upload chunks (useful for unit testing)
    void pushFilesStarted();

  public slots:
    void projectDeleted( const QString &projecFullName );
    void projectDeletedOnPath( const QString &projectDir );

  private slots:
    void listProjectsReplyFinished();

    // Pull slots
    void updateInfoReplyFinished();
    void continueDownloadFiles( const QString &projectName, const QString &version, int chunkNo = 0 );
    void downloadFileReplyFinished();

    // Push slots
    void uploadStartReplyFinished();
    void uploadInfoReplyFinished();
    void uploadFileReplyFinished();
    void uploadFinishReplyFinished();
    void uploadCancelReplyFinished();
    void continueWithUpload( const QString &projectDir, const QString &projectName, bool successfully = true );

    void getUserInfoFinished();
    void saveAuthData();
    void createProjectFinished();
    void deleteProjectFinished();
    void authorizeFinished();
    void pingMerginReplyFinished();
    void updateProjectMetadata( const QString &projectNamespace, const QString &projectName, bool syncSuccessful = true );
    void copyTempFilesToProject( const QString &projectDir, const QString &projectFullName );

  private:
    static MerginProject readProjectMetadata( const QByteArray &data );
    ProjectList parseAllProjectsMetadata();
    ProjectList parseListProjectsMetadata( const QByteArray &data );
    QJsonDocument createProjectMetadataJson( std::shared_ptr<MerginProject> project );
    static QStringList generateChunkIdsForSize( qint64 fileSize );
    QJsonArray prepareUploadChangesJSON( const QList<MerginFile> &files );

    /** Called when download has failed (aborted by user or due to network error) to clean up */
    void updateFailed( const QString &projectFullName );

    /**
     * Sends non-blocking GET request to the server to download a file (chunk).
     * \param projectFullName Namespace/name
     * \param filename Name of file to be downloaded
     * \param version version of file to be downloaded
     * \param chunkNo Chunk number of given file to be downloaded
     */
    void downloadFile( const QString &projectFullName, const QString &filename, const QString &version, int chunkNo = 0 );

    /**
     * Sends non-blocking POST request to the server to upload a file (chunk).
     * \param projectFullName Namespace/name
     * \param json project info containing metadata for upload
     */
    void uploadStart( const QString &projectFullName, const QByteArray &json );

    /**
     * Sends non-blocking POST request to the server to upload a file (chunk).
     * \param projectFullName Namespace/name
     * \param transactionUUID Transaction ID which servers sends on uploadStart
     * \param file Mergin file to upload
     * \param chunkNo Chunk number of given file to be uploaded
     */
    void uploadFile( const QString &projectFullName, const QString &transactionUUID, MerginFile file, int chunkNo = 0 );

    /**
     * Closing request after successful upload.
     * \param projectFullName Namespace/name
     * \param transactionUUID transaction UUID to match upload process on the server
     */
    void uploadFinish( const QString &projectFullName, const QString &transactionUUID );

    void sendUploadCancelRequest( const QString &projectFullName, const QString &transactionUUID );

    bool writeData( const QByteArray &data, const QString &path );
    void handleOctetStream( const QByteArray &data, const QString &projectDir, const QString &filename, bool closeFile, bool overwrite );
    bool saveFile( const QByteArray &data, QFile &file, bool closeFile, bool overwrite = false );
    void createPathIfNotExists( const QString &filePath );
    void createEmptyFile( const QString &path );
    void takeFirstAndDownload( const QString &projectFullName, const QString &version );
    /**
    *
    * \param localUpdated Timestamp version of local copy of the project
    * \param updated Timestamp version of latest project on a server
    * \param lastSync Timestamp of last successfull sync with a server
    * \param lastMod Timestamp of last modification on local copy of the project
    */
    ProjectStatus getProjectStatus( std::shared_ptr<MerginProject> project, const QDateTime &lastMod );
    QDateTime getLastModifiedFileDateTime( const QString &path );
    int getProjectFilesCount( const QString &path );
    bool isInIgnore( const QFileInfo &info );
    QByteArray getChecksum( const QString &filePath );
    QSet<QString> listFiles( const QString &projectPath );
    /**
    * Updates merginProjects list with given list. Suppose to be called after listProject request.
    * \param serverProjects List of mergin projects to be merged with current merginList project.
    */
    ProjectList updateMerginProjectList( const ProjectList &serverProjects );
    void loadAuthData();
    bool validateAuthAndContinute();
    void checkMerginVersion( QString apiVersion, QString msg = QStringLiteral() );
    /**
    * Sets projectNamespace and projectName from sourceString - url or any string from which takes last (name)
    * and the previous of last (namespace) substring after splitting sourceString with slash.
    * \param sourceString QString either url or fullname of a project
    * \param projectNamespace QString to be set as namespace, might not change original value
    * \param projectName QString to be set to name of a project
    */
    bool extractProjectName( const QString &sourceString, QString &projectNamespace, QString &projectName );
    /**
    * Extracts detail (message) of an error json. If its not json or detail cannot be parsed, the whole data are return;
    * \param data Data received from mergin server on a request failed.
    */
    QString extractServerErrorMsg( const QByteArray &data );
    /**
    * Returns mergin project directory according projectNamespace and projectName. If it is unknown, it returns empty string.
    * \param projectNamespace QString to find a project according namespace as a part of full project name
    * \param projectName QString to find a project according name as a part of full project name
    */
    QString getProjectDir( const QString &projectNamespace, const  QString &projectName );
    /**
    * Returns a temporary project path.
    * \param projectFullName
    */
    QString getTempProjectDir( const QString &projectFullName );
    /**
    * Returns given path if doesn't exists, otherwise the slightly modified non-existing path by adding a number to given path.
    * \param QString path
    */
    QString findUniqueProjectDirectoryName( QString path );
    QNetworkReply *getProjectInfo( const QString &projectFullName );

    //! Updates mergin project's sync status and emits syncProgressUpdated() signal
    void updateProjectSyncProgress( const QString &projectFullName, qreal progress );

    //! Creates a unique project directory for given project name (used for initial download of a project)
    QString createUniqueProjectDirectory( const QString &projectName );

    //! Called when download/update of project data has finished to finalize things and emit sync finished signal
    void finalizeProjectUpdate( const QString &projectFullName );

    /**
    * Used to store metadata about projects inbetween info and sync_data request.
    * MerginProjects list is updated with those data only if transfer has been successful.
    */
    QHash<QString, MerginProject> mTempMerginProjects;

    QNetworkAccessManager mManager;
    QString mApiRoot;
    ProjectList mMerginProjects;
    QString mDataDir; // dir with all projects
    QString mUsername;
    QString mPassword;
    int mUserId = -1;
    QByteArray mAuthToken;
    QDateTime mTokenExpiration;
    int mDiskUsage = 0; // in Bytes
    int mStorageLimit = 0; // in Bytes

    //! our custom attribute(s) for network requests
    static const QNetworkRequest::Attribute AttrProjectFullName = QNetworkRequest::User;

    QHash<QString, TransactionStatus> mTransactionalStatus; //projectFullname -> transactionStatus
    QSet<QString> mIgnoreExtensions = QSet<QString>() << "gpkg-shm" << "gpkg-wal" << "qgs~" << "qgz~" << "pyc" << "swap";
    QSet<QString> mIgnoreFiles = QSet<QString>() << "mergin.json" << ".DS_Store";
    QEventLoop mAuthLoopEvent;
    MerginApiStatus::VersionStatus mApiVersionStatus = MerginApiStatus::VersionStatus::UNKNOWN;

    static const int CHUNK_SIZE = 65536;
    static const int UPLOAD_CHUNK_SIZE = 10 * 1024 * 1024; // Should be the same as on Mergin server
    const QString TEMP_FOLDER = QStringLiteral( ".temp/" );

    friend class TestMerginApi;
};

#endif // MERGINAPI_H
