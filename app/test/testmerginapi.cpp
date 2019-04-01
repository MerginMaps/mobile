#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QSignalSpy>

#define STR1(x)  #x
#define STR(x)  STR1(x)

#include "testmerginapi.h"

TestMerginApi::TestMerginApi(MerginApi* api, MerginProjectModel* mpm, ProjectModel* pm, QObject* parent) {
    mApi = api;
    mMerginProjectModel = mpm;
    mProjectModel = pm;

    initTestCase();

    testListProject();
    testDownloadProject();
    testDeleteProject();

    cleanupTestCase();
}
void TestMerginApi::initTestCase()
{
    if (mApi) {
        if (::getenv("TEST_MERGIN_URL")) {
           mApiRoot = ::getenv("TEST_MERGIN_URL");
        }
        if (::getenv("TEST_API_USERNAME")) {
           mUsername = ::getenv("TEST_API_USERNAME");
        }
        if (::getenv("TEST_API_PASSWORD")) {
           mPassword = ::getenv("TEST_API_PASSWORD");
        }
        mApi->setApiRoot(mApiRoot);
        mApi->authorize(mUsername, mPassword);
    }
}

void TestMerginApi::testListProject()
{
    QSignalSpy spy(mApi, SIGNAL(listProjectsFinished(ProjectList)));
    mApi->listProjects(false);

    QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 1);

    ProjectList projects = mMerginProjectModel->projects();
    Q_ASSERT(!mMerginProjectModel->projects().isEmpty());
    qDebug() << "TestMerginApi::testListProjectFinished PASSED";
}

void TestMerginApi::testDownloadProject()
{
    QSignalSpy spy(mApi, SIGNAL(syncProjectFinished(QString, QString, bool)));
    mProjectName = "mobile_demo_mod"; // TODO depends on mergin test server, unless a project is created beforehand
    mApi->downloadProject(mProjectName);

    QVERIFY(spy.wait(5000));
    QCOMPARE(spy.count(), 1);

    ProjectList projects = mMerginProjectModel->projects();
    Q_ASSERT(!mMerginProjectModel->projects().isEmpty());
    mProjectName = mMerginProjectModel->projects().at(0)->name;
    qDebug() << "TestMerginApi::testDownloadProject PASSED";
}

void TestMerginApi::testCreateProject()
{
    QSignalSpy spy(mApi, SIGNAL(projectCreated()));
    mProjectName = "TEMPORARY_TEST_PROJECT";
    mApi->createProject(mProjectName);

    QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 1);

    ProjectList projects = getProjectList();
    Q_ASSERT(!mMerginProjectModel->projects().isEmpty());

    bool containsTestProject = false;
    for (std::shared_ptr<MerginProject> project: projects) {
        if (project->name == mProjectName) containsTestProject = true;
    }
    Q_ASSERT(containsTestProject);
    qDebug() << "TestMerginApi::testCreateProject PASSED";
}

void TestMerginApi::testDeleteProject()
{
    testCreateProject();

    QSignalSpy spy(mApi, SIGNAL(serverProjectDeleted()));
    mProjectName = "TEMPORARY_TEST_PROJECT";
    mApi->deleteProject(mProjectName);
    spy.wait(1000);

    ProjectList projects = getProjectList();
    bool containsTestProject = false;
    for (std::shared_ptr<MerginProject> project: projects) {
        if (project->name == mProjectName) containsTestProject = true;
    }
    Q_ASSERT(!containsTestProject);
    qDebug() << "TestMerginApi::testDeleteProject PASSED";
}

void TestMerginApi::cleanupTestCase()
{
    QDir testDir(mProjectModel->dataDir());
    testDir.removeRecursively();
}

ProjectList TestMerginApi::getProjectList()
{
    QSignalSpy spy(mApi, SIGNAL(listProjectsFinished(ProjectList)));
    bool withFilter = false;
    mApi->listProjects(withFilter);
    spy.wait(1000);

    return mApi->projects();
}
