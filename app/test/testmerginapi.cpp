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
    mApi->listProjects();
    QSignalSpy spy(mApi, SIGNAL(listProjectsFinished(ProjectList)));

    QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 1);

    ProjectList projects = mMerginProjectModel->projects();
    Q_ASSERT(!mMerginProjectModel->projects().isEmpty());
    mProjectName = mMerginProjectModel->projects().at(0)->name;
    qDebug() << "TestMerginApi::testListProjectFinished PASSED";
}

void TestMerginApi::testDownloadProject()
{
    QSignalSpy spy(mApi, SIGNAL(syncProjectFinished(QString, QString, bool)));
    mApi->downloadProject(mProjectName);

    QVERIFY(spy.wait(5000));
    QCOMPARE(spy.count(), 1);

    ProjectList projects = mMerginProjectModel->projects();
    Q_ASSERT(!mMerginProjectModel->projects().isEmpty());
    mProjectName = mMerginProjectModel->projects().at(0)->name;
    qDebug() << "TestMerginApi::testListProjectFinished PASSED";
}

void TestMerginApi::cleanupTestCase()
{
    QDir testDir(mProjectModel->dataDir());
    testDir.removeRecursively();
}
