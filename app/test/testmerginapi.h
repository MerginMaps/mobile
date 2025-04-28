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

#include "inputconfig.h"
#include <qgsvectorlayer.h>
#include <merginapi.h>
#include <projectsmodel.h>
#include "project.h"

#include <qgsapplication.h>

class MockReply : public QNetworkReply
{
  public:
    explicit MockReply( const QNetworkRequest &request, QNetworkAccessManager::Operation operation,
                        QObject *parent = nullptr, QNetworkReply::NetworkError errorCode = QNetworkReply::NoError )
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
      open( QIODevice::ReadOnly );
    }

    void abort() override {}

    qint64 readData( char *data, qint64 maxlen ) override
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

    void setShouldFail( bool shouldFail, QNetworkReply::NetworkError errorCode = QNetworkReply::NoError )
    {
      mShouldFail = shouldFail;
      mErrorCode = errorCode;
    }

  protected:
    QNetworkReply *createRequest( Operation op, const QNetworkRequest &request, QIODevice *outgoingData = nullptr ) override
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
    ~TestMerginApi();

    static const QString TEST_PROJECT_NAME;
    static const QString TEST_EMPTY_FILE_NAME;

  private slots:

    // global init + cleanup functions
    void initTestCase();
    void cleanupTestCase();

    void testListProject();
    void testListProjectsByName();
    void testDownloadProject();
    void testDownloadWithNetworkError();
    void testDownloadWithNetworkErrorRecovery();
    void testDownloadProjectSpecChars();
    void testCancelDownloadProject();
    void testCreateProjectTwice();
    void testDeleteNonExistingProject();
    void testCreateDeleteProject();
    void testUploadProject();
    void testMultiChunkUploadDownload();
    void testEmptyFileUploadDownload();
    void testPushAddedFile();
    void testPushRemovedFile();
    void testPushModifiedFile();
    void testPushNoChanges();
    void testUpdateAddedFile();
    void testUpdateRemovedFiles();
    void testUpdateRemovedVsModifiedFiles();
    void testConflictRemoteUpdateLocalUpdate();
    void testConflictRemoteAddLocalAdd();
    void testEditConflictScenario();
    void testUploadWithUpdate();
    void testDiffUpload();
    void testDiffSubdirsUpload();
    void testDiffUpdateBasic();
    void testDiffUpdateWithRebase();
    void testDiffUpdateWithRebaseFailed();
    void testUpdateWithDiffs();
    void testUpdateWithMissedVersion();
    void testMigrateProject();
    void testMigrateProjectAndSync();
    void testMigrateDetachProject();
    void testSelectiveSync();
    void testSelectiveSyncSubfolder();
    void testSelectiveSyncAddConfigToExistingProject();
    void testSelectiveSyncRemoveConfig();
    void testSelectiveSyncDisabledInConfig();
    void testSelectiveSyncChangeSyncFolder();
    void testSelectiveSyncCorruptedFormat();
    void testSynchronizationViaManager();
    void testAutosync();
    void testAutosyncFailure();
    void testUpdateProjectMetadataRole();
    void testMerginConfigFromFile();
    void testHasLocalChangesWithSelectiveSyncEnabled();
    void testHasLocalProjectChanges();

    void testRegisterAndDelete();
    void testCreateWorkspace();

    // mergin functions
    void testExcludeFromSync();

    void testServerType();

    void testServerUpgrade();

    void testServerError();

    void testRegistration();

    void testParseVersion();

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

    MerginProjectsList getProjectList( QString tag = "created" );
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
     * If project does not exists, it does nothing.
     */
    void deleteRemoteProjectNow( MerginApi *api, const QString &projectNamespace, const QString &projectName );

    //! Downloads a remote project to the local drive, extended version also sets server version
    void downloadRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName, int &serverVersion );
    void downloadRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName );

    //! Uploads any local changes in the local project to the remote project, extended version also sets server version
    void uploadRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName, int &serverVersion );
    void uploadRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName );

    //! Deletes a project from the local drive
    void deleteLocalProject( MerginApi *api, const QString &projectNamespace, const QString &projectName );

    //! Recursively deletes directory and its content.
    void deleteLocalDir( MerginApi *api, const QString &dirPath );

    //! Write all of "data" as the content to the given filename
    void writeFileContent( const QString &filename, const QByteArray &data );
    //! Read content of the given filename
    QByteArray readFileContent( const QString &filename );

    //! Creates local project in given project directory
    void createLocalProject( const QString projectDir );

    //! Creates json file based on params in path. Returns true is successful, false otherwise
    bool createJsonFile( const QString &path, const QVariantMap &params );

    void refreshProjectsModel( const ProjectsModel::ProjectModelTypes modelType = ProjectsModel::LocalProjectsModel );
};

# endif // TESTMERGINAPI_H
