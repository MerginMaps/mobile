#include "merginapi.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>

MerginApi::MerginApi(const QString &root, MerginProjectModel *model, QObject *parent)
  : QObject (parent)
  , mApiRoot(root)
  , mModel(model)
{
}

void MerginApi::listProjects()
{
    mMerginProjects.clear();
    QNetworkRequest request;
    // projects filtered by tag "input_use"
    QUrl url(mApiRoot + "/v1/project?tags=input_use");

    request.setUrl(url);
    request.setRawHeader("Authorization", QByteArray("Basic ") + mToken);

    QNetworkReply *reply = mManager.get(request);
    connect(reply, &QNetworkReply::finished, this, &MerginApi::listProjectsReplyFinished);
}

void MerginApi::listProjectsReplyFinished()
{

    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(r);

    if (r->error() == QNetworkReply::NoError)
    {
      QByteArray data = r->readAll();
      qDebug("R: %s", data.constData());
      mMerginProjects = parseProjectsData(data);
    }
    else {
        QString message = QStringLiteral("Network API error: %1(): %2").arg("listProjects", r->errorString());
        qDebug("%s", message.toStdString().c_str());
    }


    mModel->resetProjects(mMerginProjects);
    r->deleteLater();
    emit listProjectsFinished();
}

ProjectList MerginApi::parseProjectsData(QByteArray data)
{
    ProjectList result;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isArray()) {
        QJsonArray vArray = doc.array();

        for ( auto it = vArray.constBegin(); it != vArray.constEnd(); ++it)
        {
            QJsonObject designMap = it->toObject();
            MerginProject p;
            // get info from server
            p.name = designMap.value("name").toString();
            QString created = designMap.value("created").toString();
            QDateTime dateTime = QDateTime::fromString(created, Qt::ISODateWithMs);
            p.info = dateTime.toString();
            result << std::make_shared<MerginProject>(p);
        }
    }
    return result;
}
