#include "merginapi.h"

#include <QtNetwork>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>
#include <memory>

#ifdef ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#endif

MerginApi::MerginApi(const QString &root, QByteArray token, QObject *parent)
  : QObject (parent)
  , mApiRoot(root)
  , mToken(token)
{
}

void MerginApi::listProjects()
{
    mMerginProjects.clear();
    QNetworkRequest request;
    // projects filtered by tag "input_use"
    QUrl url(mApiRoot + "/v1/project?tags=input_use");

    request.setUrl(url);
    request.setRawHeader("Authorization", QByteArray("Basic " + mToken));

    QNetworkReply *reply = mManager.get(request);
    connect(reply, &QNetworkReply::finished, this, &MerginApi::listProjectsReplyFinished);
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
      mMerginProjects = parseProjectsData(data);
    }
    else {
        // TODO propagate error
        QString message = QStringLiteral("Network API error: %1(): %2").arg("listProjects", r->errorString());
        qDebug("%s", message.toStdString().c_str());
        emit networkErrorOccurred( r->errorString() );
        QString errMsg = r->errorString() ;
#ifdef ANDROID
        QtAndroid::runOnAndroidThread([errMsg] {
            QAndroidJniObject javaString = QAndroidJniObject::fromString(errMsg);
            QAndroidJniObject toast = QAndroidJniObject::callStaticObjectMethod("android/widget/Toast", "makeText",
                                                                                "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;",
                                                                                QtAndroid::androidActivity().object(),
                                                                                javaString.object(),
                                                                                jint(1));
            toast.callMethod<void>("show");
        });
#endif
    }

    r->deleteLater();
    emit listProjectsFinished();
}

ProjectList MerginApi::parseProjectsData(const QByteArray data)
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
