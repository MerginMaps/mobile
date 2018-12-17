#ifndef MERGINAPI_H
#define MERGINAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <memory>
#include <QFile>

struct MerginProject {
    QString name;
    QStringList tags;
    QString info;
    bool pending = false;
};
typedef QList<std::shared_ptr<MerginProject>> ProjectList;

class MerginApi: public QObject {
    Q_OBJECT
public:
    explicit MerginApi(const QString& root, const QString& dataDir, QByteArray token, QObject* parent = nullptr );
    ~MerginApi() = default;
    Q_INVOKABLE void listProjects();
    Q_INVOKABLE void downloadProject(QString projectName);
    ProjectList projects();

signals:
    void listProjectsFinished();
    void downloadProjectFinished(QString projectFolder, QString projectName);
    void networkErrorOccurred(QString message, QString additionalInfo);
    void notify(QString message);

private slots:
    void listProjectsReplyFinished();
    void downloadProjectReplyFinished();

private:
    ProjectList parseProjectsData( const QByteArray &data );
    void makeToast(const QString &errorMessage, const QString &additionalInfo);
    QString saveFileName(const QUrl &url);
    void unzipProject(QString path, QString dir);
    bool cacheProjectsData(const QByteArray &data);

    QNetworkAccessManager mManager;
    QString mApiRoot;
    ProjectList mMerginProjects;
    QString mDataDir;
    QByteArray mToken;
    QHash<QUrl, QString>mPendingRequests;

    QString createProjectFile( const QByteArray data, QString projectName );
};

#endif // MERGINAPI_H
