#ifndef TESTMERGINAPI_H
#define TESTMERGINAPI_H

#include <QObject>

#include <qgsvectorlayer.h>
#include <merginapi.h>
#include <projectsmodel.h>
#include <merginprojectmodel.h>

#include <qgsapplication.h>

class TestMerginApi: public QObject
{
    Q_OBJECT
  public:
    explicit TestMerginApi( MerginApi *api, MerginProjectModel *mpm, ProjectModel *pm );
    ~TestMerginApi() = default;

    static const QString TEST_PROJECT_NAME;

  private slots:

    // global init + cleanup functions
    void initTestCase();
    void cleanupTestCase();

    void testListProject();
    void testDownloadProject();
    void testCancelDownloadProject();
    void testCreateProjectTwice();
    void testDeleteNonExistingProject();
    void testCreateDeleteProject();
    void testUploadProject();
    void testMultiChunkUploadDownload();
    void testPushAddedFile();
    void testPushRemovedFile();
    void testPushModifiedFile();
    void testUpdateAddedFile();
    void testUpdateRemovedFiles();
    void testUpdateRemovedVsModifiedFiles();
    void testConflictRemoteUpdateLocalUpdate();
    void testConflictRemoteAddLocalAdd();

  private:
    static const int SHORT_REPLY = 1000;
    static const int LONG_REPLY = 5000;

    MerginApi *mApi;
    MerginProjectModel *mMerginProjectModel;
    ProjectModel *mProjectModel;
    QString mUsername;
    QString mTestDataPath;
    //! extra API to do requests we are not testing (as if some other user did those)
    MerginApi *mApiExtra = nullptr;
    LocalProjectsManager *mLocalProjectsExtra = nullptr;

    MerginProjectList getProjectList();

    //! Creates a project on the server and pushes an initial version and removes the local copy.
    void createRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName, const QString &sourcePath );
    //! Deletes a project on the server
    void deleteRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName );

    //! Downloads a remote project to the local drive
    void downloadRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName, int timeout = LONG_REPLY * 5 );

    //! Uploads any local changes in the local project to the remote project
    void uploadRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName, int timeout = LONG_REPLY );

    //! Deletes a project from the local drive
    void deleteLocalProject( MerginApi *api, const QString &projectNamespace, const QString &projectName );

    //! Write all of "data" as the content to the given filename
    void writeFileContent( const QString &filename, const QByteArray &data );
    //! Read content of the given filename
    QByteArray readFileContent( const QString &filename );
};

# endif // TESTMERGINAPI_H
