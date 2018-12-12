#include "merginapi.h"
#include "zlib.h"

#include <QtNetwork>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>
//#include <JlCompress.h>

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
    connect( this, &MerginApi::networkErrorOccurred, this, &MerginApi::makeToast );
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

void MerginApi::downloadProject(QString projectName)
{
    mMerginProjects.clear();
    QNetworkRequest request;
    // TODO check spaces in names
    QUrl url(mApiRoot + "/v1/project/download/" + projectName + "?format=zip");
    qDebug() << "Requested " << url.toString();

    request.setUrl(url);
    request.setRawHeader("Authorization", QByteArray("Basic " + mToken));

    QNetworkReply *reply = mManager.get(request);
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
      mMerginProjects = parseProjectsData(data);
    }
    else {
        emit networkErrorOccurred( r->errorString(), "Mergin API error: listProjects" );
    }

    r->deleteLater();
    emit listProjectsFinished();
}

QString MerginApi::createProjectFile(const QByteArray data, QString projectName)
{
    // TODO test mDataDir
    QDir dir;
    if (!dir.exists(mDataDir))
        dir.mkpath(mDataDir);
    QFile file(mDataDir + projectName);
    if (!file.exists()) {
        qDebug("File doesn't exists yet");
    } else {
        // TODO overwrite projects??
    }

    int size = data.size();
    qDebug("size: %s", QString::number(size).toStdString().c_str());
    qDebug("RES: %s", data.toStdString().c_str());

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

    if (r->error() == QNetworkReply::NoError)
    {


        QString projectName = "test";
        QString extention = ".zip";
        QString filePath = createProjectFile(r->readAll(), projectName + extention);

        //        // TODO Unzip
        //        void unZip(QString zip_filename , QString filename)
        //        {


        QDir dir;
        if (!dir.exists(mDataDir))
            dir.mkdir(mDataDir);
        QFile infile(mDataDir + projectName + extention);
        QFile outfile(mDataDir + projectName);
        QuaZip zip("zipFile.zip");
        zip.open(QuaZip::mdUnzip);

        QuaZipFile file(&zip);


//        infile.open(QIODevice::ReadOnly);
//        outfile.open(QIODevice::WriteOnly);
//        QByteArray uncompressed_data = infile.readAll();
//        QByteArray compressed_data = qUncompress(uncompressed_data);
//        outfile.write(compressed_data);
//        infile.close();

    }
    else {
        emit networkErrorOccurred( r->errorString(), "Mergin API error: downloadProject" );
    }

    r->deleteLater();
    emit downloadProjectFinished();
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

void MerginApi::makeToast(const QString &errorMessage, const QString &additionalInfo)
{
    QString message = QStringLiteral("%1(): %2").arg(additionalInfo, errorMessage);
    qDebug("%s", message.toStdString().c_str());
#ifdef ANDROID
        QtAndroid::runOnAndroidThread([errorMessage] {
            QAndroidJniObject javaString = QAndroidJniObject::fromString(errorMessage);
            QAndroidJniObject toast = QAndroidJniObject::callStaticObjectMethod("android/widget/Toast", "makeText",
                                                                                "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;",
                                                                                QtAndroid::androidActivity().object(),
                                                                                javaString.object(),
                                                                                jint(1));
            toast.callMethod<void>("show");
        });
#endif
}

// TODO
QString MerginApi::getDataDir() {
//#ifdef QGIS_QUICK_DATA_PATH
//  QString dataPathRaw(STR(QGIS_QUICK_DATA_PATH));

//#ifdef ANDROID
//  QFileInfo extDir("/sdcard/");
//  if(extDir.isDir() && extDir.isWritable()){
//      // seems that this directory transposes to the latter one in case there is no sdcard attached
//      dataPathRaw = extDir.path() + "/" + dataPathRaw;
//  } else {
//      qDebug() << "extDir: " << extDir.path() << " not writable";

//      QStringList split = QDir::homePath().split("/"); // something like /data/user/0/com.gigaclear/files
//      // TODO support active user from QDir::homePath()
//      QFileInfo usrDir("/storage/emulated/" + split[2] + "/");
//      dataPathRaw = usrDir.path() + "/" + dataPathRaw;
//      if(!(usrDir.isDir() && usrDir.isWritable())){
//          qDebug() << "usrDir: " << usrDir.path() << " not writable";
//      }
//  }
//#endif

//  ::setenv("QGIS_QUICK_DATA_PATH", dataPathRaw.toUtf8().constData(), true);
//#else
//  qDebug("== Must set QGIS_QUICK_DATA_PATH in order to get QGIS Quick running! ==");
//#endif
  QString dataDir(::getenv("QGIS_QUICK_DATA_PATH"));
  qDebug() << "QGIS_QUICK_DATA_PATH: " << dataDir;
  return "/Users/vsklencar/lutra/mobile/input/app/android/assets/qgis-data";//dataDir;
}
