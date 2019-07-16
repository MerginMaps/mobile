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
    static const QString TEST_PROJECT_NAME_DOWNLOAD;

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
    void testPushRemovedFile();
    void testPushChangesOfProject();

    void testParseAndCompareNoChanges();
    void testParseAndCompareRemovedAdded();
    void testParseAndCompareUpdated();
    void testParseAndCompareRenamed();

  private:
    int SHORT_REPLY = 1000;
    int LONG_REPLY = 5000;

    MerginApi *mApi;
    MerginProjectModel *mMerginProjectModel;
    ProjectModel *mProjectModel;
    QString mUsername;
    QString mTestDataPath;

    ProjectList getProjectList();
    void initTestProject();
    std::shared_ptr<MerginProject> prepareTestProjectUpload();

    //! Creates a project on the server and pushes an initial version and removes the local copy.
    void createRemoteProject( const QString &projectNamespace, const QString &projectName, const QString &sourcePath );
    //! Deletes a project on the server
    void deleteRemoteProject( const QString &projectNamespace, const QString &projectName );

    //! Deletes a project from the local drive
    void deleteLocalProject( const QString &projectNamespace, const QString &projectName );
};

# endif // TESTMERGINAPI_H
