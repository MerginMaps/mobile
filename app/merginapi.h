#ifndef MERGINAPI_H
#define MERGINAPI_H

#include "merginprojectmodel.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>

class MerginApi: public QObject {
     Q_OBJECT
public:
    explicit MerginApi(const QString& root, MerginProjectModel *model, QByteArray token, QObject* parent = nullptr );
    ~MerginApi() = default;
    Q_INVOKABLE void listProjects();

signals:
    void listProjectsFinished();

private slots:
    void listProjectsReplyFinished();

private:
    ProjectList parseProjectsData( QByteArray data );

    QEventLoop mEventLoop;
    QNetworkAccessManager mManager;
    QString mApiRoot;
    ProjectList mMerginProjects;
    MerginProjectModel *mModel;
    QByteArray mToken;

};

#endif // MERGINAPI_H
