#ifndef MERGINAPI_H
#define MERGINAPI_H

#include <projectsmodel.h>

#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>

struct MerginProject {
    QString name;
    QStringList tags;
    QDate craeted;
};
typedef QList<std::shared_ptr<MerginProject>> ProjectList;

class MerginApi: public QObject {
     Q_OBJECT
public:
    explicit MerginApi(const QString& root, ProjectModel *model, QObject* parent = nullptr );
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
    ProjectModel *mModel;
    QByteArray mToken = QByteArray("TODO");

};

#endif // MERGINAPI_H
