#include <QtTest/QtTest>
#include <QtCore/QObject>

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
    QObject::connect(mApi, &MerginApi::listProjectsFinished, this, &TestMerginApi::testListProjectFinished);
    mApi->listProjects();
    mEventLoop.exec();
}

void TestMerginApi::testDownloadProject()
{
    QObject::connect(mApi, &MerginApi::syncProjectFinished, this, &TestMerginApi::testDownloadProjectFinished);
    mApi->downloadProject(mProjectName);
    mEventLoop.exec();
}

void TestMerginApi::cleanupTestCase()
{
    QDir testDir(mProjectModel->dataDir());
    testDir.removeRecursively();
}

void TestMerginApi::testListProjectFinished()
{
    mEventLoop.quit();
    ProjectList projects = mMerginProjectModel->projects();
    Q_ASSERT(!mMerginProjectModel->projects().isEmpty());
    mProjectName = mMerginProjectModel->projects().at(0)->name;
    QObject::disconnect(mApi, &MerginApi::listProjectsFinished, this, &TestMerginApi::testListProjectFinished);
    qDebug() << "TestMerginApi::stestListProjectFinished PASSED";
}

void TestMerginApi::testDownloadProjectFinished()
{
    int projectCountAfter = mProjectModel->rowCount();
    Q_ASSERT(1 == projectCountAfter);
    mEventLoop.quit();
    QObject::disconnect(mApi, &MerginApi::syncProjectFinished, this, &TestMerginApi::testDownloadProjectFinished);
    qDebug() << "TestMerginApi::testDownloadProjectFinished PASSED";
}
