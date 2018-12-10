#ifndef MERGINAPI_H
#define MERGINAPI_H

#include "merginprojectmodel.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>

class MerginApi: public QObject {
     Q_OBJECT
public:
    explicit MerginApi(const QString& root, MerginProjectModel *model, QObject* parent = nullptr );
    ~MerginApi() = default;
    Q_INVOKABLE void reloadModel();
    ProjectList listProjects();

signals:
    void listProjectsFinished();

private slots:
    void listProjectsReplyFinished();

private:
    ProjectList parseProjectsData( QByteArray data );

    QEventLoop mEventLoop;
    QNetworkAccessManager mManager;
    QString mApiRoot; //e.g. localhost:5000
    ProjectList mMerginProjects;
    MerginProjectModel *mModel;
    QByteArray mToken = QByteArray("TODO");

};

#endif // MERGINAPI_H
