/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTMERGINAPI_H
#define TESTMERGINAPI_H

#include <QObject>
#include <QSignalSpy>

#include "qgsvectorlayer.h"
#include "merginapi.h"
#include "projectsmodel.h"
#include "project.h"

class MockReply : public QNetworkReply
{
  public:
    explicit MockReply( const QNetworkRequest &request, const QNetworkAccessManager::Operation operation,
                        QObject *parent = nullptr, const QNetworkReply::NetworkError errorCode = QNetworkReply::NoError )
      : QNetworkReply( parent )
    {
      setRequest( request );
      setOperation( operation );
      setUrl( request.url() );

      if ( errorCode != QNetworkReply::NoError )
      {
        setError( errorCode, "Mock network failure" );
        QMetaObject::invokeMethod( this, "errorOccurred", Qt::QueuedConnection, Q_ARG( QNetworkReply::NetworkError, errorCode ) );
      }

      QMetaObject::invokeMethod( this, "finished", Qt::QueuedConnection );
      QIODevice::open( QIODevice::ReadOnly );
    }

    void abort() override {}

    qint64 readData( char *data, const qint64 maxlen ) override
    {
      Q_UNUSED( data );
      Q_UNUSED( maxlen );
      return -1;
    }

    qint64 bytesAvailable() const override
    {
      return 0;
    }
};

class MockNetworkManager : public QNetworkAccessManager
{
  public:
    explicit MockNetworkManager( QObject *parent = nullptr )
      : QNetworkAccessManager( parent )
      , mShouldFail( false )
      , mErrorCode( QNetworkReply::NoError )
    {}

    void setShouldFail( const bool shouldFail, const QNetworkReply::NetworkError errorCode = QNetworkReply::NoError )
    {
      mShouldFail = shouldFail;
      mErrorCode = errorCode;
    }

  protected:
    QNetworkReply *createRequest( const Operation op, const QNetworkRequest &request, QIODevice *outgoingData = nullptr ) override
    {
      if ( mShouldFail )
      {
        auto *reply = new MockReply( request, op, this, mErrorCode );
        return reply;
      }
      return QNetworkAccessManager::createRequest( op, request, outgoingData );
    }

  private:
    bool mShouldFail;
    QNetworkReply::NetworkError mErrorCode;
};

class TestMerginApi: public QObject
{
    Q_OBJECT
  public:
    explicit TestMerginApi( MerginApi *api );
    ~TestMerginApi() override;

    static const QString TEST_PROJECT_NAME;
    static const QString TEST_EMPTY_FILE_NAME;

  private slots:

    // global init + cleanup functions
    void initTestCase();
    void cleanupTestCase();

    /**
     * Test first deletes the test project on server if it exists and the creates a new empty project.
     */
    void testListProject();

    /**
     * Test creates new project on server and then fetches it by \code listProjectsByName \endcode API.
     */
    void testListProjectsByName();

    /**
     * Test creates a project on server, downloads the project from a scratch using \code pullProject \endcode API
     * and checks the integrity of local project.
     */
    void testDownloadProject();

    /**
     * Test creates new project on server, defines network errors and mocks them via \a MockNetworkManager.
     */
    void testDownloadWithNetworkError();

    /**
     * Test creates new project on server and tries to download it. Two network errors are mocked from, which
     * should the process recover.
     */
    void testDownloadWithNetworkErrorRecovery();

    /**
     * Test creates new project on server and tries to upload and download files with special characters in name.
     */
    void testDownloadProjectSpecChars();

    /**
     * Test creates new project on server and cancels download before API is called and then after API is called.
     */
    void testCancelDownloadProject();

    /**
     * Test creates new project on server and then tries to create the same one again.
     */
    void testCreateProjectTwice();

    /**
     * Test tries to delete non-existent project from server.
     */
    void testDeleteNonExistingProject();

    /**
     * Test creates new project on server and deletes it.
     */
    void testCreateDeleteProject();

    /**
     * Test creates new project on server and tries to upload data to server. First it gets canceled right away,
     * secondly it gets canceled when data starts to get uploaded, thirdly it gets uploaded.
     */
    void testUploadProject();

    /**
     * Test creates new project on server. Tries to upload a file that needs to be split into multiple chunks
     * and then also downloads it correctly again in a clean new download.
     */
    void testMultiChunkUploadDownload();

    /**
     * Test creates new project on server. Tries to upload and download empty file.
     */
    void testEmptyFileUploadDownload();

    /**
     * Test creates new project on server. Downloads it, adds a file, uploads it, deletes local copy
     * and downloads it again.
     */
    void testPushAddedFile();

    /**
     * Test creates new project on server. Downloads it, removes a file, uploads it, deletes local copy
     * and downloads it again.
     */
    void testPushRemovedFile();

    /**
     * Test creates new project on server. Downloads it, modifies a file, uploads it, deletes local copy
     * and downloads it again.
     */
    void testPushModifiedFile();

    /**
     * Test creates new project on server. Downloads it, uploads it back without changes.
     */
    void testPushNoChanges();

    /**
     * Test creates new project on server. Downloads it, new file is created on server, checks the server version,
     * downloads the newer version.
     */
    void testUpdateAddedFile();

    /**
     * Test creates new project on server. Downloads it, file is removed on server, checks the server version,
     * downloads the newer version.
     */
    void testUpdateRemovedFiles();

    /**
     * Test creates new project on server. Downloads it, file is removed on server, but also modified locally,
     * checks the server version, downloads the newer version. In the end we keep the modified version.
     */
    void testUpdateRemovedVsModifiedFiles();

    /**
     * This test downloads a project, makes a local update of a file, in the meanwhile it does remote update of
     * the same file to create a conflict. Finally, it tries to upload the local change to test the code
     * responsible for conflict resolution (renames the local file).
     */
    void testConflictRemoteUpdateLocalUpdate();

    /**
     * This test downloads a project, creates a new file in the meanwhile it creates the same file on
     * the server to create a conflict. Finally, it tries to upload the local change to test the code
     * responsible for conflict resolution (renames the local file).
     */
    void testConflictRemoteAddLocalAdd();

    /**
     * This test simulates creation of edit conflict when two clients are trying to update the same attribute.
     * Edit conflict file should be created inside project folder and synced to server
     */
    void testEditConflictScenario();

    /**
     * This test triggers the situation when the request to upload a project first needs to do an update and
     * only afterwards it uploads changes.
     */
    void testUploadWithUpdate();

    /**
     * This test creates a new project on server, downloads it, afterwards makes changes to gpkg and uploads it.
     */
    void testDiffUpload();

    /**
     * This test creates a new project on server, downloads it, afterwards makes changes to gpkg in nested
     * subdirectory and uploads it.
     */
    void testDiffSubdirsUpload();

    /**
     * This test creates a new project on server, downloads it. Another device downloads it too and uploads modified
     * version of gpkg. Newer version is downloaded again.
     */
    void testDiffUpdateBasic();

    /**
     * This test creates a new project on server, downloads it. Another device downloads it too and uploads modified
     * version of gpkg. Newer version is downloaded again and local changes are rebased on top of the server's change.
     */
    void testDiffUpdateWithRebase();

    /**
     * This test creates a new project on server, downloads it. Another device downloads it too and uploads modified
     * version of gpkg. Newer version is downloaded again and local changes are rebased on top of the server's change.
     * \note Test case where the local change is something that geodiff does not support and thus cannot rebase
     * the changes (should create a conflict file instead).
     */
    void testDiffUpdateWithRebaseFailed();

    /**
     * Test case where we download initial version (v1), then there will be two versions with diffs (v2 and v3),
     * afterwards we try to update the local project.
     */
    void testUpdateWithDiffs();

    /**
     * When updating from v3 to v4, it is expected that we will get references to diffs for v2-v3 and v3-v4.
     * There was a bug where we always ignored the first one. But it could happen that there is no update in v2-v3,
     * and we ended up ignoring v3-v4, ending up with broken base files.
     *
     * 1. [extra] create project, upload .gpkg (v1)
     * 2. [extra] upload a new file (v2)
     * 3. [main]  download project (v2)
     * 4. [extra] upload updated .gpkg (v3)
     * 5. [main]  update from v2 to v3
     */
    void testUpdateWithMissedVersion();

    /**
     * Test creates a local project and uploads it to the server.
     */
    void testMigrateProject();

    /**
     * When a new project is migrated to Mergin, creating base files for diffable files was omitted. Therefore,
     * sync was not properly working resulting into having a conflict file. Test covers creating a new project,
     * migrating it to Mergin and both sides sync.
     *
     * 1. [main] create project with .gpkg (v1) file
     * 2. [main] migrate the project to mergin
     * 3. [extra] download the project and make changes to .gpkg (v2)
     * 4. [main] sync the project (v2), should be valid without conflicts
     * 5. [main] make changes to .gpkg (v3) and sync
     * 6. [extra] sync the project (v3), should be valid without conflicts
     */
    void testMigrateProjectAndSync();

    /**
     * Test creates local project, uploads it to server and then removes from server, but keeps the local part.
     */
    void testMigrateDetachProject();

    /**
     * Case: Clients have the following configuration: selective sync on, selective-sync-dir empty
     * (project dir by default)
     *
     * Action 1: Client 1 uploads some images and Client 2 sync without downloading the images
     *
     * Action 2: Client 2 uploads an image and do not remove not-synced images. Client 1 syncs without downloading
     * the image, still having own images.
     */
    void testSelectiveSync();

    /**
     * Case: Downloading project with config.
     *
     * We have the following scenario:
     * {
     *   "input-selective-sync": true,
     *   "input-selective-sync-dir": "photos" // having subfolder
     * }
     *
     * Action 1: Client 1 creates project with mergin-config and uploads some images,
     *           Client 2 should sync without downloading the images.
     *
     * Action 2: Client 2 uploads two images, one in "photos" subdirectory and second in project root.
     *           Client 1 should sync without downloading the image in "photos" subdirectory and should still have
     *           own images (they should not be deleted even though Client 2 did not have them when syncing)
     */
    void testSelectiveSyncSubfolder();

    /**
     * Case: Have a project with photos without mergin config, add it when both clients are using project already to simulate
     *       users that add mergin config to existing projects.
     *
     * Procedure: Create project with photos, sync it to both clients, then let Client 1 add mergin config together with several
     *            pictures and see if the new pictures are NOT synced.
     */
    void testSelectiveSyncAddConfigToExistingProject();

    /**
     * Case: Remove mergin-config from an existing project with photos.
     *
     * We will create another API client that will serve as a server mirror, it will not use selective sync,
     * but will simulate as if someone manipulated project from browser via Mergin
     */
    void testSelectiveSyncRemoveConfig();

    /**
     * Case: Disable selective sync in mergin-config in an existing project with photos and selective sync previously enabled.
     *
     * We will create another API client that will serve as a server mirror, it will not use selective sync,
     * but will simulate as if someone manipulated project from browser via Mergin
     */
    void testSelectiveSyncDisabledInConfig();

    /**
     * Case: Change selective sync folder in mergin-config in an existing project with photos and selective sync enabled.
     *
     * We will create another API client that will serve as a server mirror, it will not use selective sync,
     * but will simulate as if someone manipulated project from browser via Mergin
     */
    void testSelectiveSyncChangeSyncFolder();

    /**
     * Case: Test what happens when someone uploads not valid config file (not valid json)
     *
     * We will create another API client that will serve as a server mirror, it will not use selective sync,
     * but will simulate as if someone manipulated project from browser via Mergin
     */
    void testSelectiveSyncCorruptedFormat();

    /**
     * 1. instantiate sync manager
     * 2. create remote project & download it
     * 3. add some data
     * 4. sync it via manager
     * 5. check if all signals are called
     */
    void testSynchronizationViaManager();

    /**
     * 1. copy test project to temp
     * 2. allow autosync controller
     * 3. load the project
     * 4. make some changes in the project
     * 5. make sure autosync controller triggers that data has changed
     */
    void testAutosync();

    /**
     * 1. copy test project to temp
     * 2. load it
     * 3. create autosync controller
     * 4. sign out
     * 5. make some changes in the project
     * 6. make sure autosync controller has correct failure state
     * 7. sign back in
     * \todo Will be added with incremental requests
     */
    void testAutosyncFailure();

    /**
     * Test creates new project on server and downloads it. Then project role is changed locally and again
     * fetched from server.
     */
    void testUpdateProjectMetadataRole();

    /**
     * Test tries to load \a MerginConfig from file.
     */
    void testMerginConfigFromFile();

    /**
     * Test creates \a MerginConfig with selective sync enabled. Two variants are tested, files included in sync and
     * files not included in sync.
     */
    void testHasLocalChangesWithSelectiveSyncEnabled();

    /**
     * Test multiple scenarios to register local changes.
     * 1. Empty metadata and no local files, selective sync not supported
     * 2. Metadata has files and no local files, selective sync not supported
     * 3. Metadata files equals local files, selective sync supported
     * 4. Local files differs from metadata, selective sync supported
     */
    void testHasLocalProjectChanges();

    void testOfflineCache();

    /**
     * Test creates new user and deletes it.
     */
    void testRegisterAndDelete();

    /**
     * Test creates new user, new workspace and tries to delete the user (expects to fail).
     */
    void testCreateWorkspace();

    // mergin functions

    /**
     * Test creates \a MerginConfig and checks if Mergin API parses it correctly.
     */
    void testExcludeFromSync();

    /**
     * Test querying server config.
     */
    void testServerType();

    /**
     * Test setting server type.
     */
    void testServerUpgrade();

    /**
     * Test parsing server errors.
     */
    void testServerError() const;

    /**
     * Test registration checks.
     */
    void testRegistration();

    /**
     * Test parsing version string.
     */
    void testParseVersion() const;
    void testApiRoot();

  private:
    MerginApi *mApi = nullptr;

    std::unique_ptr<ProjectsModel> mLocalProjectsModel;
    std::unique_ptr<ProjectsModel> mWorkspaceProjectsModel;
    std::unique_ptr<SynchronizationManager> mSyncManager;
    QString mUsername;
    QString mWorkspaceName;
    QString mTestDataPath;
    //! extra API to do requests we are not testing (as if some other user did those)
    MerginApi *mApiExtra = nullptr;
    LocalProjectsManager *mLocalProjectsExtra = nullptr;

    MerginProjectsList getProjectList( const QString &tag = "created" );
    MerginProjectsList projectListFromSpy( QSignalSpy &spy );
    int serverVersionFromSpy( QSignalSpy &spy );

    /**
     *  Creates a project on the server and pushes an initial version and removes the local copy.
     *  If force is used, the project is first deleted by deleteRemoteProjectNow from remote server
     */
    void createRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName, const QString &sourcePath, bool force = true );

    /**
     * Gets project id from project full name
     * Returns empty string if project is not found on server
     */
    QString projectIdFromProjectFullName( MerginApi *api, const QString &projectNamespace, const QString &projectName );

    /**
     * Immediately deletes a project on the server
     * If project does not exist, it does nothing.
     */
    void deleteRemoteProjectNow( MerginApi *api, const QString &projectNamespace, const QString &projectName );

    //! Downloads a remote project to the local drive, extended version also sets server version
    void downloadRemoteProject( MerginApi *api, const QString &projectFullName, const QString &projectId, int &serverVersion );
    void downloadRemoteProject( MerginApi *api, const QString &projectFullName, const QString &projectId );

    //! Uploads any local changes in the local project to the remote project, extended version also sets server version
    void uploadRemoteProject( MerginApi *api, const QString &projectFullName, const QString &projectId, int &serverVersion );
    void uploadRemoteProject( MerginApi *api, const QString &projectFullName, const QString &projectId );

    //! Deletes a project from the local drive
    void deleteLocalProject( const MerginApi *api, const QString &projectId );

    //! Recursively deletes directory and its content.
    void deleteLocalDir( const MerginApi *api, const QString &dirPath );

    //! Write all of "data" as the content to the given filename
    void writeFileContent( const QString &filename, const QByteArray &data );
    //! Read content of the given filename
    QByteArray readFileContent( const QString &filename );

    //! Creates local project in given project directory
    void createLocalProject( const QString &projectDir );

    //! Creates json file based on params in path. Returns true is successful, false otherwise
    bool createJsonFile( const QString &path, const QVariantMap &params );

    void refreshProjectsModel( const ProjectsModel::ProjectModelTypes modelType = ProjectsModel::LocalProjectsModel );
};

# endif // TESTMERGINAPI_H
