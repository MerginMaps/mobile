#include <QtTest/QtTest>
#include <QtCore/QObject>

#define STR1(x)  #x
#define STR(x)  STR1(x)

#include "testmerginapi.h"

TestMerginApi::TestMerginApi(MerginApi* api, MerginProjectModel* mpm, QObject* parent) {
    mApi = api;
    mMerginProjectModel = mpm;

    initTestCase();
    testListProject();
    testDownloadProject();
    cleanupTestCase();
}
void TestMerginApi::initTestCase()
{
    if (mApi) {
#ifdef TEST_API
        mApiRoot = STR(TEST_API);
        mApiRoot = "https://" + mApiRoot;
#endif
#ifdef TEST_API_USERNAME
        mUsername = STR(TEST_API_USERNAME);
#endif
#ifdef TEST_API_PASSWORD
        mPassword = STR(TEST_API_PASSWORD);
#endif
        mApi->setApiRoot(mApiRoot);
        mApi->authorize(mUsername, mPassword);
        qDebug() << mUsername << mPassword;
        // TODO test for authorize
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
    QString projectName = "mobile_demo_mod";
    QObject::connect(mApi, &MerginApi::syncProjectFinished, this, &TestMerginApi::testDownloadProjectFinished);
    mApi->downloadProject(projectName);
    mEventLoop.exec();
}

void TestMerginApi::cleanupTestCase()
{
    // TODO cleanup private members
    //smEventLoop.quit();
}

void TestMerginApi::testListProjectFinished()
{
    mEventLoop.quit();
    QObject::disconnect(mApi, &MerginApi::listProjectsFinished, this, &TestMerginApi::testListProjectFinished);
}

void TestMerginApi::testDownloadProjectFinished()
{
    int projectCountAfter = mMerginProjectModel->projects().length();
    qDebug() << "TESTING PROJECT COUNT AFTER DOWNLOAD:" << projectCountAfter;
    Q_ASSERT(1 == projectCountAfter);
    mEventLoop.quit();
    QObject::disconnect(mApi, &MerginApi::syncProjectFinished, this, &TestMerginApi::testDownloadProjectFinished);
}
