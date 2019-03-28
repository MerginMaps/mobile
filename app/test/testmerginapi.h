#ifndef TESTMERGINAPI_H
#define TESTMERGINAPI_H

#include <QObject>
#include <QEventLoop>

#include <qgsvectorlayer.h>
#include <merginapi.h>
#include <projectsmodel.h>
#include <merginprojectmodel.h>

#include <qgsapplication.h>

class TestMerginApi: public QObject
{
    Q_OBJECT
 public:
    explicit TestMerginApi(MerginApi* api, MerginProjectModel* mpm, QObject* parent = nullptr);
    ~TestMerginApi() = default;

  public slots:
    void initTestCase();
    void testListProject();
    void testDownloadProject();
    void cleanupTestCase();

    void testListProjectFinished();
    void testDownloadProjectFinished();


private:
    MerginApi *mApi;
    MerginProjectModel *mMerginProjectModel;
    QString mApiRoot;
    QString mUsername;
    QString mPassword;

    QEventLoop mEventLoop;
};

# endif // TESTMERGINAPI_H
