#include "merginapi.h"
#include "qgsziputils.h"

#include <QtNetwork>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>

#ifdef ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#endif

MerginApi::MerginApi(const QString &root, const QString& dataDir, QByteArray token, QObject *parent)
  : QObject (parent)
  , mApiRoot(root)
  , mDataDir(dataDir + "/downloads/")
  , mToken(token)
{
}

void MerginApi::listProjects()
{
    if (mToken.isEmpty()) {
        emit networkErrorOccurred( "Auth token is invalid", "Mergin API error: listProjects" );
        return;
    }

    mMerginProjects.clear();
    QNetworkRequest request;
    // projects filtered by tag "input_use"
    QUrl url(mApiRoot + "/v1/project?tags=input_use");
    request.setUrl(url);
    request.setRawHeader("Authorization", QByteArray("Basic " + mToken));

    QNetworkReply *reply = mManager.get(request);
    connect(reply, &QNetworkReply::finished, this, &MerginApi::listProjectsReplyFinished);
}

void MerginApi::downloadProject(QString projectName)
{
    if (mToken.isEmpty()) {
        emit networkErrorOccurred( "Auth token is invalid", "Mergin API error: downloadProject" );
    }

    mMerginProjects.clear();
    QNetworkRequest request;
    QUrl url(mApiRoot + "/v1/project/download/" + projectName + "?format=zip");
    qDebug() << "Requested " << url.toString();

    if (mPendingRequests.contains(url)) {
        QString errorMsg = QStringLiteral("Download request for %1 is already pending.").arg(projectName);
        qDebug() << errorMsg;
        emit networkErrorOccurred( errorMsg, "Mergin API error: downloadProject" );
        return;
    }

    request.setUrl(url);
    request.setRawHeader("Authorization", QByteArray("Basic " + mToken));

    QNetworkReply *reply = mManager.get(request);
    mPendingRequests.insert(url, projectName);
    connect(reply, &QNetworkReply::finished, this, &MerginApi::downloadProjectReplyFinished);
}

ProjectList MerginApi::projects()
{
    return mMerginProjects;
}

void MerginApi::listProjectsReplyFinished()
{

    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(r);

    if (r->error() == QNetworkReply::NoError)
    {
      QByteArray data = r->readAll();
      cacheProjectsData(data);
      mMerginProjects = parseProjectsData(data);
    }
    else {
        QString message = QStringLiteral("Network API error: %1(): %2").arg("listProjects", r->errorString());
        qDebug("%s", message.toStdString().c_str());
        emit networkErrorOccurred( r->errorString(), "Mergin API error: listProjects" );
    }

    r->deleteLater();
    emit listProjectsFinished();
}

QString MerginApi::createProjectFile(const QByteArray data, QString projectName)
{
    QDir dir;
    if (!dir.exists(mDataDir))
        dir.mkpath(mDataDir);
    QFile file(mDataDir + projectName);
    if (file.exists()) {
        qDebug("Zip file already exists!");
        // TODO overwrite projects / handle data-sync
    }

    bool isOpen = file.open(QIODevice::WriteOnly);
    QString path;
    if (isOpen) {
        file.write(data);
        file.close();
        path = QFileInfo(file).absoluteFilePath();
    } else {
        qDebug() << "File cannot be open";
    }

    return path;
}

void MerginApi::downloadProjectReplyFinished()
{

    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(r);

    QString projectName("temp");
    if (mPendingRequests.contains(r->url())) {
        projectName = mPendingRequests.value(r->url());
    }

    if (r->error() == QNetworkReply::NoError)
    {
        QString extention = ".zip";
        QString filePath = createProjectFile(r->readAll(), projectName + extention);
        QString projectDir = mDataDir + projectName;
        unzipProject(filePath, projectDir + "/");
        emit downloadProjectFinished(projectDir, projectName);
        emit notify("Download successful");
    }
    else {
        emit networkErrorOccurred( r->errorString(), "Mergin API error: downloadProject" );
    }
    mPendingRequests.remove(r->url());
    r->deleteLater();
}

ProjectList MerginApi::parseProjectsData(const QByteArray &data)
{
    ProjectList result;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isArray()) {
        QJsonArray vArray = doc.array();

        for ( auto it = vArray.constBegin(); it != vArray.constEnd(); ++it)
        {
            QJsonObject designMap = it->toObject();
            MerginProject p;
            p.name = designMap.value("name").toString();
            QString created = designMap.value("created").toString();
            p.info = QDateTime::fromString(created, Qt::ISODateWithMs).toString();
            result << std::make_shared<MerginProject>(p);
        }
    }
    return result;
}

QString MerginApi::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    if (QFile::exists(basename)) {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }

    return basename;
}

void MerginApi::unzipProject(QString path, QString dir)
{
    QDir d;
    if (!d.exists(dir))
        d.mkdir(dir);

    QStringList files;
    QgsZipUtils::unzip(path, dir, files);
}

bool MerginApi::cacheProjectsData(const QByteArray &data)
{
    QDir dir;
    if (!dir.exists(mDataDir))
        dir.mkpath(mDataDir);

    QFile file(mDataDir + "projectsCache.txt");
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream stream(&file);
    stream << data;
    file.close();

    return true;
}
