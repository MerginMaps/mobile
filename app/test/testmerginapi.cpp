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
    testCreateProjectTwice();
    testDeleteNonExistingProject();
    testCreateDeleteProject();

    cleanupTestCase();
    qDebug() << "TestMerginApi - ALL TEST PASSED";
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
    qDebug() << "TestMerginApi::testListProjectFinished START";
    QSignalSpy spy(mApi, SIGNAL(listProjectsFinished(ProjectList)));
    mApi->listProjects(QString());

    QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 1);

    ProjectList projects = mMerginProjectModel->projects();
    Q_ASSERT(!mMerginProjectModel->projects().isEmpty());
    qDebug() << "TestMerginApi::testListProjectFinished PASSED";
}

void TestMerginApi::testDownloadProject()
{
    qDebug() << "TestMerginApi::testDownloadProject START";
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

void TestMerginApi::testCreateProjectTwice()
{
    qDebug() << "TestMerginApi::testCreateProjectTwice START";
    mProjectName = "TEMPORARY_TEST_PROJECT";
    bool containsTestProject = false;
    ProjectList projects = getProjectList();
    for (std::shared_ptr<MerginProject> project: projects) {
        if (project->name == mProjectName) containsTestProject = true;
    }
    Q_ASSERT(!containsTestProject);

    QSignalSpy spy(mApi, SIGNAL(projectCreated()));
    mApi->createProject(mProjectName);
    QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 1);

    projects = getProjectList();
    Q_ASSERT(!mMerginProjectModel->projects().isEmpty());

    for (std::shared_ptr<MerginProject> project: projects) {
        if (project->name == mProjectName) containsTestProject = true;
    }
    Q_ASSERT(containsTestProject);

    // Create again, expecting error
    QSignalSpy spy2(mApi, SIGNAL(networkErrorOccurred(QString, QString)));
    mApi->createProject(mProjectName);
    QVERIFY(spy2.wait(1000));
    QCOMPARE(spy2.count(), 1);

    QList<QVariant> arguments = spy2.takeFirst();
    QVERIFY(arguments.at(0).type() == QVariant::String);
    QVERIFY(arguments.at(1).type() == QVariant::String);

    QCOMPARE(arguments.at(1), QStringLiteral("Mergin API error: createProject"));

    //Clean created project
    QSignalSpy spy3(mApi, SIGNAL(networkErrorOccurred(QString, QString)));
    mApi->deleteProject(mProjectName);
    spy3.wait(1000);

    containsTestProject = false;
    projects = getProjectList();
    for (std::shared_ptr<MerginProject> project: projects) {
        if (project->name == mProjectName) containsTestProject = true;
    }
    Q_ASSERT(!containsTestProject);

    qDebug() << "TestMerginApi::testCreateProjectTwice PASSED";
}

void TestMerginApi::testDeleteNonExistingProject()
{
    qDebug() << "TestMerginApi::testDeleteNonExistingProject START";
    // Checks if projects doestn exist
    mProjectName = "TEMPORARY_TEST_PROJECT";
    bool containsTestProject = false;
    ProjectList projects = getProjectList();
    for (std::shared_ptr<MerginProject> project: projects) {
        if (project->name == mProjectName) containsTestProject = true;
    }
    Q_ASSERT(!containsTestProject);

    // Try to delete non-existing project
    QSignalSpy spy(mApi, SIGNAL(networkErrorOccurred(QString, QString)));
    mApi->deleteProject(mProjectName);
    spy.wait(1000);

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).type() == QVariant::String);
    QVERIFY(arguments.at(1).type() == QVariant::String);
    QCOMPARE(arguments.at(1), QStringLiteral("Mergin API error: deleteProject"));

    qDebug() << "TestMerginApi::testDeleteNonExistingProject PASSED";
}

void TestMerginApi::testCreateDeleteProject()
{
    qDebug() << "TestMerginApi::testCreateDeleteProject START";
    // Create a project
    mProjectName = "TEMPORARY_TEST_PROJECT";
    bool containsTestProject = false;

    ProjectList projects = getProjectList();
    for (std::shared_ptr<MerginProject> project: projects) {
        if (project->name == mProjectName) containsTestProject = true;
    }
    Q_ASSERT(!containsTestProject);

    QSignalSpy spy(mApi, SIGNAL(projectCreated()));
    mApi->createProject(mProjectName);
    QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 1);

    projects = getProjectList();
    Q_ASSERT(!mMerginProjectModel->projects().isEmpty());

    for (std::shared_ptr<MerginProject> project: projects) {
        if (project->name == mProjectName) containsTestProject = true;
    }
    Q_ASSERT(containsTestProject);

    // Delete created project
    QSignalSpy spy2(mApi, SIGNAL(serverProjectDeleted(QString)));
    mApi->deleteProject(mProjectName);
    spy.wait(1000);

    projects = getProjectList();
    containsTestProject = false;
    for (std::shared_ptr<MerginProject> project: projects) {
        if (project->name == mProjectName) containsTestProject = true;
    }
    Q_ASSERT(!containsTestProject);
    qDebug() << "TestMerginApi::testCreateDeleteProject PASSED";
}

void TestMerginApi::cleanupTestCase()
{
    QDir testDir(mProjectModel->dataDir());
    testDir.removeRecursively();
}

ProjectList TestMerginApi::getProjectList()
{
    QSignalSpy spy(mApi, SIGNAL(listProjectsFinished(ProjectList)));
    mApi->listProjects(QString());
    spy.wait(1000);

    return mApi->projects();
}
