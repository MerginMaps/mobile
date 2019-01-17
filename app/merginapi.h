#ifndef MERGINAPI_H
#define MERGINAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <memory>
#include <QFile>

enum ProjectStatus
{
  NoVersion,
  UpToDate,
  OutOfDate
};
Q_ENUMS( ProjectStatus )

struct MerginProject {
    QString name;
    QStringList tags;
    QDateTime created;
    QDateTime updated; // last local update
    QDateTime serverUpdated; // last update on server
    bool pending = false; // if there is a pending request for downlaod/update a project
    ProjectStatus status = NoVersion;
};

struct MerginFile {
    QString path;
    QString checksum;
};


typedef QList<std::shared_ptr<MerginProject>> ProjectList;

class MerginApi: public QObject {
    Q_OBJECT
public:
    explicit MerginApi(const QString& root, const QString& dataDir, QByteArray token, QObject* parent = nullptr );
    ~MerginApi() = default;

    /**
     * Sends non-blocking GET request to the server to listProjects. On listProjectsReplyFinished,
     * when a response is received, parses project json, writes it to a cache text file and sets mMerginProjects.
     * Eventually emits listProjectsFinished on which ProjectPanel (qml component) updates content.
     * If listing has been successful, updates cached merginProjects list.
     * @param
     */
    Q_INVOKABLE void listProjects();

    /**
     * Sends non-blocking GET request to the server to download a project with a given name. On downloadProjectReplyFinished,
     * when a response is received, parses data-stream and creates files. Eventually emits downloadProjectFinished on which
     * MerginProjectModel updates status of the project item. On downloadProjectFinished, ProjectModel adds the project item to the project list.
     * If download has been successful, updates cached merginProjects list.
     * Emits also notify signal with a message for the GUI.
     * @param projectName Name of project to download.
     */
    Q_INVOKABLE void downloadProject(QString projectName);

    /**
     * Sends non-blocking GET request to the server to update a project with a given name. On downloadProjectReplyFinished,
     * when a response is received, parses data-stream to files and rewrites local files with them. Extra files which don't match server
     * files are removed. Eventually emits downloadProjectFinished on which MerginProjectModel updates status of the project item.
     * If update has been successful, updates cached merginProjects list.
     * Emits also notify signal with a message for the GUI.
     * @param projectName Name of project to update.
     */
    Q_INVOKABLE void updateProject(QString projectName);

    Q_INVOKABLE void uploadProject(QString projectName);

    ProjectList projects();

signals:
    void listProjectsFinished(ProjectList merginProjects);
    void downloadProjectFinished(QString projectDir, QString projectName);
    void updateProjectFinished(QString projectDir, QString projectName);
    void networkErrorOccurred(QString message, QString additionalInfo);
    void notify(QString message);
    void merginProjectsChanged();

private slots:
    void listProjectsReplyFinished();
    void downloadProjectReplyFinished(); // download + update
    void uploadProjectReplyFinished();
    void updateInfoReplyFinished();
    void uploadInfoReplyFinished();
    void cacheProjects();

private:
    ProjectList parseProjectsData(const QByteArray &data, bool dataFromServer = false);
    bool cacheProjectsData(const QByteArray &data);
    void handleDataStream(QNetworkReply* r, QString projectDir);
    bool saveFile(const QByteArray &data, QFile &file, bool closeFile);
    void createPathIfNotExists(QString filePath);
    ProjectStatus getProjectStatus(QDateTime localUpdated, QDateTime updated);
    QByteArray getChecksum(QString filePath);
    QSet<QString> listFiles(QString projectPath);
    void downloadProjectFiles(QString projectName, QByteArray json);
    void uploadProjectFiles(QString projectName, QByteArray json, QList<MerginFile> files);
    QHash<QString, QList<MerginFile>> parseAndCompareProjectFiles(QNetworkReply *r);
    ProjectList updateMerginProjectList(ProjectList serverProjects);
    void setUpdateToProject(QString projectName);
    void deleteObsoleteFiles(QString projectName);

    QNetworkAccessManager mManager;
    QString mApiRoot;
    ProjectList mMerginProjects;
    QString mDataDir;
    QString mCacheFile;
    QByteArray mToken;
    QHash<QUrl, QString>mPendingRequests;
    QHash<QString, QSet<QString>> mObsoleteFiles;

    const int CHUNK_SIZE = 65536;
};

#endif // MERGINAPI_H
