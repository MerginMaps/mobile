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

#include <qgsvectorlayer.h>
#include <merginapi.h>
#include <projectsmodel.h>
#include "project.h"

#include <qgsapplication.h>

class TestMerginApi: public QObject
{
    Q_OBJECT
  public:
    explicit TestMerginApi( MerginApi *api );
    ~TestMerginApi() = default;

    static const QString TEST_PROJECT_NAME;
    static const QString TEST_EMPTY_FILE_NAME;

  private slots:

    // global init + cleanup functions
    void initTestCase();
    void cleanupTestCase();

    void testListProject();
    void testListProjectsByName();
    void testDownloadProject();
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

    void testRegister();

    // mergin functions
    void testExcludeFromSync();

  private:
    MerginApi *mApi;
    std::unique_ptr<ProjectsModel> mLocalProjectsModel;
    std::unique_ptr<ProjectsModel> mCreatedProjectsModel;
    QString mUsername;
    QString mTestDataPath;
    //! extra API to do requests we are not testing (as if some other user did those)
    MerginApi *mApiExtra = nullptr;
    LocalProjectsManager *mLocalProjectsExtra = nullptr;

    MerginProjectsList getProjectList( QString tag = "created" );
    MerginProjectsList projectListFromSpy( QSignalSpy &spy );
    int serverVersionFromSpy( QSignalSpy &spy );

    //! Creates a project on the server and pushes an initial version and removes the local copy.
    void createRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName, const QString &sourcePath );
    //! Deletes a project on the server
    void deleteRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName );

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
