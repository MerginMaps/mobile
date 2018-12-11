#ifndef MERGINAPI_H
#define MERGINAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>

struct MerginProject {
    QString name;
    QStringList tags;
    QString info;
};
typedef QList<std::shared_ptr<MerginProject>> ProjectList;

class MerginApi: public QObject {
     Q_OBJECT
public:
    explicit MerginApi(const QString& root, QByteArray token, QObject* parent = nullptr );
    ~MerginApi() = default;
    Q_INVOKABLE void listProjects();
    ProjectList projects();

signals:
    void listProjectsFinished();
    void networkErrorOccurred(QString errorMessage);

private slots:
    void listProjectsReplyFinished();

private:
    ProjectList parseProjectsData( const QByteArray data );

    QNetworkAccessManager mManager;
    QString mApiRoot;
    ProjectList mMerginProjects;
    QByteArray mToken;

};

#endif // MERGINAPI_H
