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
    void downloadProjectFinished();
    void networkErrorOccurred(QString errorMessage, QString additionalInfo);

private slots:
    void listProjectsReplyFinished();
    void downloadProjectReplyFinished();

private:
    ProjectList parseProjectsData( const QByteArray data );
    void makeToast(const QString &errorMessage, const QString &additionalInfo);
    QString getDataDir();
    QString saveFileName(const QUrl &url);

    QNetworkAccessManager mManager;
    QString mApiRoot;
    ProjectList mMerginProjects;
    QString mDataDir;
    QByteArray mToken;

    QString createProjectFile( const QByteArray data, QString projectName );
};

#endif // MERGINAPI_H
