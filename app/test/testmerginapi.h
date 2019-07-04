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
    explicit TestMerginApi( MerginApi *api, MerginProjectModel *mpm, ProjectModel *pm, QObject *parent = nullptr );
    ~TestMerginApi() = default;

    static const QString TEST_PROJECT_NAME;
    static const QString TEST_PROJECT_NAME_DOWNLOAD;

  public slots:
    void initTestCase();
    void testListProject();
    void testDownloadProject();
    void testCancelDownlaodProject();
    void testCreateProjectTwice();
    void testDeleteNonExistingProject();
    void testCreateDeleteProject();
    void testUploadProject();
    void testPushChangesOfProject();

    void testParseAndCompareNoChanges();
    void testParseAndCompareRemovedAdded();
    void testParseAndCompareUpdated();
    void testParseAndCompareRenamed();

    void cleanupTestCase();

  private:
    int SHORT_REPLY = 1000;
    int LONG_REPLY = 5000;

    MerginApi *mApi;
    MerginProjectModel *mMerginProjectModel;
    ProjectModel *mProjectModel;
    QString mApiRoot;
    QString mUsername;
    QString mPassword;
    QString mTestData;
    int passedTests = 0;
    int runTests = 0;

    ProjectList getProjectList();
    bool hasProject( QString projectNamespace, QString projectName, ProjectList projects );
    void initTestProject();
    std::shared_ptr<MerginProject> prepareTestProjectUpload();
    void deleteTestProjects();
    void copyTestProject();
    QString testDataPath();
    void deleteSingleTestProject( QString &projectNamespace, const QString &projectName );
};

# endif // TESTMERGINAPI_H
