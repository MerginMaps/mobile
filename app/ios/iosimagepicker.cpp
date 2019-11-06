#include <QStandardPaths>
#include <QFileDialog>
#include <QTemporaryFile>
#include <QUrl>
#include <QRunnable>
#include <QPointer>
#include <QThreadPool>
#include <QImageWriter>
#include <QImageReader>
#include "iossystemdispatcher.h"
#include "iosimagepicker.h"
#include <QtDebug>
#include <QCoreApplication>

#include "inpututils.h"

class IOSImagePickerSaver : public QRunnable {
public:
    QPointer<IOSImagePicker> owner;
    QImage image;
    QString absoluteImagePath;

    /// <#Description#>
    void run() {
        if (absoluteImagePath.isNull()) { // Save as temp
            QTemporaryFile tmp;
            QStringList paths = QStandardPaths::standardLocations(QStandardPaths::TempLocation);
            //QString tmpPath = paths.at(0);
            //QStringList paths = QCoreApplication::applicationDirPath();
            QString tmpPath = owner->targetPath();

            tmp.setFileTemplate(tmpPath + "/"+ QDateTime::currentDateTime().toString( QStringLiteral( "yyMMdd-hhmmss" ) )+ ".jpg");
            tmp.open();
            //absoluteImagePath = tmp.fileName();
            absoluteImagePath = tmpPath + "/"+ QDateTime::currentDateTime().toString( QStringLiteral( "yyMMdd-hhmmss" ) )+ ".jpg";
            //fileName = InputUtils::renameWithDateTime(fileName);
            tmp.close();

//            QString tmpPath = owner->targetPath();
//            fileName = QString("")
//            QString basename =
//            qDebug() << "BASENAME: "<< basename << "|";
//            qDebug() << "MEdia: "<< owner->mediaUrl() << "|" << owner->referenceUrl();
//            fileName = QString("%1/%2").arg(owner->targetPath(), basename);
//            qDebug() << "fileNAME: "<< fileName << "|";
        }

        image.save(absoluteImagePath);
        QImageWriter writer;
        writer.setFileName(absoluteImagePath);
        if (!writer.write(image)) {
            qWarning() << QString("Failed to save %1 : %2").arg(absoluteImagePath).arg(writer.errorString());
        }

        if (!owner.isNull()) {
            QMetaObject::invokeMethod(owner.data(),"endSave",Qt::QueuedConnection,
                                      Q_ARG(QString, absoluteImagePath));
        }
    }
};

IOSImagePicker::IOSImagePicker(QObject *parent) : QObject(parent)
{
    m_sourceType = PhotoLibrary;
    m_status = Null;
    m_busy = false;
}

IOSImagePicker::~IOSImagePicker()
{

}

void IOSImagePicker::show(bool animated)
{
    if (m_status == Running || m_status == Saving) {
        return;
    }

#ifdef Q_OS_IOS
    QISystemDispatcher* system = QISystemDispatcher::instance();

    QVariantMap data;
    data["sourceType"] = m_sourceType;
    data["animated"] = animated;

    connect(system,SIGNAL(dispatched(QString,QVariantMap)),
            this,SLOT(onReceived(QString,QVariantMap)));

    bool res = system->dispatch("imagePickerControllerPresent",data);

    if (res) {
        setStatus(Running);
    }
#endif
}

void IOSImagePicker::close(bool animated)
{
    QISystemDispatcher* system = QISystemDispatcher::instance();
    QVariantMap data;
    data["animated"] = animated;

    system->dispatch("imagePickerControllerDismiss",data);
}

void IOSImagePicker::save(QString fileName)
{
    IOSImagePickerSaver* saver = new IOSImagePickerSaver();
    saver->setAutoDelete(true);
    saver->owner = this;
    saver->absoluteImagePath = fileName;
    saver->image = m_image;

    QThreadPool::globalInstance()->start(saver);
}

void IOSImagePicker::saveAsTemp()
{
    save(QString());
}

void IOSImagePicker::clear()
{
    setImage(QImage());
    emit imageChanged();
    setStatus(Null);
}

IOSImagePicker::SourceType IOSImagePicker::sourceType() const
{
    return m_sourceType;
}

void IOSImagePicker::setSourceType(const SourceType &sourceType)
{
    m_sourceType = sourceType;
    emit sourceTypeChanged();
}

QImage IOSImagePicker::image() const
{
    return m_image;
}

void IOSImagePicker::setImage(const QImage &image)
{
    m_image = image;
    if (true) {
        QString mFilename;
        m_image.save(mFilename, "PNG");
    }

    emit imageChanged();
}

IOSImagePicker::Status IOSImagePicker::status() const
{
    return m_status;
}

void IOSImagePicker::setStatus(const Status &status)
{
    if (m_status == status)
        return;
    m_status = status;
    emit statusChanged();
}

void IOSImagePicker::onReceived(QString name, QVariantMap data)
{
    QISystemDispatcher* system = QISystemDispatcher::instance();

    if (name == "imagePickerControllerDidCancel") {
        setStatus(Null);
        system->disconnect(this);
        close();
    }

    if (name != "imagePickerControllerDisFinishPickingMetaWithInfo")
        return;

    system->disconnect(this);
    QImage image = data["image"].value<QImage>();
    setImage(image);
    setMediaType(data["mediaType"].toString());
    setMediaUrl(data["mediaUrl"].toString());
    setReferenceUrl(data["referenceUrl"].toString());

    setStatus(Ready);
    emit ready();
}

void IOSImagePicker::endSave(QString fileName)
{
    QUrl url = QUrl::fromLocalFile(fileName);

    emit saved(url.toString());

    if (m_image.isNull()) {
        setStatus(Null);
    } else {
        setStatus(Ready);
    }
}

QString IOSImagePicker::targetPath() const
{
    return mTargetPath;
}

void IOSImagePicker::setTargetPath(const QString &targetPath)
{
    mTargetPath = targetPath;
    emit targetPathChanged();
}
QString IOSImagePicker::referenceUrl() const
{
    return m_referenceUrl;
}

void IOSImagePicker::setReferenceUrl(const QString &referenceUrl)
{
    m_referenceUrl = referenceUrl;
    emit referenceUrlChanged();
}

QString IOSImagePicker::mediaUrl() const
{
    return m_mediaUrl;
}

void IOSImagePicker::setMediaUrl(const QString &mediaUrl)
{
    m_mediaUrl = mediaUrl;
    emit mediaUrlChanged();
}

QString IOSImagePicker::mediaType() const
{
    return m_mediaType;
}

void IOSImagePicker::setMediaType(const QString &mediaType)
{
    m_mediaType = mediaType;
    emit mediaTypeChanged();
}

bool IOSImagePicker::busy() const
{
    return m_busy;
}

void IOSImagePicker::setBusy(bool busy)
{
    if (m_busy == busy)
        return;

    m_busy = busy;
    QISystemDispatcher* system = QISystemDispatcher::instance();

    QVariantMap data;
    data["active"] = m_busy;

    system->dispatch("imagePickerControllerSetIndicator",data);
    emit busyChanged();
}




