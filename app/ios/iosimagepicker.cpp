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

class QIImagePickerSaver : public QRunnable {
public:
    QPointer<QIImagePicker> owner;
    QImage image;
    QString fileName;

    void run() {
        if (fileName.isNull()) { // Save as temp
            QTemporaryFile tmp;
            QStringList paths = QStandardPaths::standardLocations(QStandardPaths::TempLocation);
            QString tmpPath = paths.at(0);

            tmp.setFileTemplate(tmpPath + "/XXXXXX.jpg");
            tmp.open();
            fileName = tmp.fileName();
            tmp.close();
        }

        image.save(fileName);
        QImageWriter writer;
        writer.setFileName(fileName);
        if (!writer.write(image)) {
            qWarning() << QString("Failed to save %1 : %2").arg(fileName).arg(writer.errorString());
        }

        if (!owner.isNull()) {
            QMetaObject::invokeMethod(owner.data(),"endSave",Qt::QueuedConnection,
                                      Q_ARG(QString,fileName));
        }
    }
};

QIImagePicker::QIImagePicker(QObject *parent) : QObject(parent)
{
    m_sourceType = PhotoLibrary;
    m_status = Null;
    m_busy = false;
}

QIImagePicker::~QIImagePicker()
{

}

void QIImagePicker::show(bool animated)
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

#else
    Q_UNUSED(animated);
    // For desktop preview
    qDebug() << "DESKTOP PREIVEW!!!!!!";
    setStatus(Running);
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);

    QString file = QFileDialog::getOpenFileName (0,
                                                 tr("Import Image"),
                                                 paths.at(0),
                                                 "Images (*.png *.xpm *.jpg)");

    if (file.isNull()) {
        setStatus(Null);
    } else {
        QImageReader reader;
        reader.setFileName(file);
        QImage image = reader.read();

        if (image.isNull()) {
            setStatus(Null);
        } else {
            QUrl url = QUrl::fromLocalFile(file);

            setImage(image);
            setMediaType("public.image");
            setReferenceUrl(url.toString());

            setStatus(Ready);
            emit ready();
        }
    }
#endif
}

void QIImagePicker::close(bool animated)
{
    QISystemDispatcher* system = QISystemDispatcher::instance();
    QVariantMap data;
    data["animated"] = animated;

    system->dispatch("imagePickerControllerDismiss",data);
}

void QIImagePicker::save(QString fileName)
{
    QIImagePickerSaver* saver = new QIImagePickerSaver();
    saver->setAutoDelete(true);
    saver->owner = this;
    saver->fileName = fileName;
    saver->image = m_image;

    QThreadPool::globalInstance()->start(saver);
}

void QIImagePicker::saveAsTemp()
{
    save(QString());
}

void QIImagePicker::clear()
{
    setImage(QImage());
    emit imageChanged();
    setStatus(Null);
}

QIImagePicker::SourceType QIImagePicker::sourceType() const
{
    return m_sourceType;
}

void QIImagePicker::setSourceType(const SourceType &sourceType)
{
    m_sourceType = sourceType;
    emit sourceTypeChanged();
}

QImage QIImagePicker::image() const
{
    return m_image;
}

void QIImagePicker::setImage(const QImage &image)
{
    m_image = image;
    if (true) {
        QString mFilename;
        m_image.save(mFilename, "PNG");
    }

    emit imageChanged();
}

QIImagePicker::Status QIImagePicker::status() const
{
    return m_status;
}

void QIImagePicker::setStatus(const Status &status)
{
    if (m_status == status)
        return;
    m_status = status;
    emit statusChanged();
}

void QIImagePicker::onReceived(QString name, QVariantMap data)
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

void QIImagePicker::endSave(QString fileName)
{
    QUrl url = QUrl::fromLocalFile(fileName);

    emit saved(url.toString());

    if (m_image.isNull()) {
        setStatus(Null);
    } else {
        setStatus(Ready);
    }
}
QString QIImagePicker::referenceUrl() const
{
    return m_referenceUrl;
}

void QIImagePicker::setReferenceUrl(const QString &referenceUrl)
{
    m_referenceUrl = referenceUrl;
    emit referenceUrlChanged();
}

QString QIImagePicker::mediaUrl() const
{
    return m_mediaUrl;
}

void QIImagePicker::setMediaUrl(const QString &mediaUrl)
{
    m_mediaUrl = mediaUrl;
    emit mediaUrlChanged();
}

QString QIImagePicker::mediaType() const
{
    return m_mediaType;
}

void QIImagePicker::setMediaType(const QString &mediaType)
{
    m_mediaType = mediaType;
    emit mediaTypeChanged();
}

bool QIImagePicker::busy() const
{
    return m_busy;
}

void QIImagePicker::setBusy(bool busy)
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




