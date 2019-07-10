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
  NoVersion,
  UpToDate,
  OutOfDate,
  Modified
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

struct TransactionStatus
{
  qreal totalSize = 0;
  int transferedSize = 0;
  QString transactionUUID; // only for upload
  QNetworkReply *openReply = nullptr; // all sync replies except cancel
  QList<MerginFile> files; // either to upload or download
};

struct MerginProject
{
  QString name;
  QString projectNamespace;
  QString projectDir;
  QStringList tags;
  QList<MerginFile> files;
  QString version;
  QDateTime clientUpdated; // client's version of project files
  QDateTime serverUpdated; // available latest version of project files on server
  QDateTime lastSyncClient; // local datetime of download/upload/update project
  bool pending = false; // if there is a pending request for downlaod/update a project
  ProjectStatus status = NoVersion;
  int size;
  int filesCount;
  qreal progress;
  int creator; // ID of current user
  QList<int> writers;
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

    static const int MERGIN_API_VERSION_MAJOR = 2019;
    static const int MERGIN_API_VERSION_MINOR = 4;
    static const QString sMetadataFile;

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

  signals:
    void listProjectsFinished( const ProjectList &merginProjects );
    void listProjectsFailed();
    void syncProjectFinished( const QString &projectDir, const QString &projectFullName, bool successfully = true );
    void syncProgressUpdated( const QString &projectFullName, qreal progress );
    void downloadFileFinished( const QString &projectFullName, const QString &version, int chunkNo = 0, bool successfully = true );
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

  public slots:
    void projectDeleted( const QString &projecFullName );
    void projectDeletedOnPath( const QString &projectDir );

  private slots:
    void listProjectsReplyFinished();

    // Pull slots
    void updateInfoReplyFinished();
    void continueDownloadFiles( const QString &projectName, const QString &version, int chunkNo = 0, bool successfully = true );
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
    static QString defaultApiRoot() { return "https://public.cloudmergin.com/"; }
    static MerginProject readProjectMetadata( const QByteArray &data );
    ProjectList parseAllProjectsMetadata();
    ProjectList parseListProjectsMetadata( const QByteArray &data );
    QJsonDocument createProjectMetadataJson( std::shared_ptr<MerginProject> project );
    QStringList generateChunkIdsForSize( qint64 fileSize );
    QJsonArray prepareUploadChangesJSON( const QList<MerginFile> &files, bool onlyPath = false );

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

    bool writeData( const QByteArray &data, const QString &path );
    void handleOctetStream( const QByteArray &data, const QString &projectDir, const QString &filename, bool closeFile, bool overwrite );
    bool saveFile( const QByteArray &data, QFile &file, bool closeFile, bool overwrite = false );
    void createPathIfNotExists( const QString &filePath );
    void createEmptyFile( const QString &path );
    void takeFirstAndDownload( const QString &projectFullName, const QString &version );
    void deleteReply( QNetworkReply *r, const QString &projectFullName );
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
    void deleteObsoleteFiles( const QString &projectPath );
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
    * Returns mergin project directory according projectNamespace and projectName. Either it already exists
    * or it creates a new folder for the project and returns its path.
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

    QHash<QUrl, QString >mPendingRequests; // url -> projectNamespace/projectName
    QHash<QString, QSet<QString>> mObsoleteFiles; // projectPath -> files
    QSet<QString> mWaitingForUpload; // projectNamespace/projectName
    QHash<QString, TransactionStatus> mTransactionalStatus; //projectFullname -> transactionStatus
    QSet<QString> mIgnoreExtensions = QSet<QString>() << "gpkg-shm" << "gpkg-wal" << "qgs~" << "qgz~" << "pyc" << "swap";
    QSet<QString> mIgnoreFiles = QSet<QString>() << "mergin.json" << ".DS_Store";
    QEventLoop mAuthLoopEvent;
    MerginApiStatus::VersionStatus mApiVersionStatus = MerginApiStatus::VersionStatus::UNKNOWN;

    const int CHUNK_SIZE = 65536;
    const int UPLOAD_CHUNK_SIZE = 10 * 1024 * 1024; // Should be the same as on Mergin server
    const QString TEMP_FOLDER = QStringLiteral( ".temp/" );
};

#endif // MERGINAPI_H
