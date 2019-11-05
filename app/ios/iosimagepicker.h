#ifndef QIIMAGEPICKER_H
#define QIIMAGEPICKER_H

#include <QObject>
#include <QImage>

#include <QVariant>
#include <QString>
#include <QMap>
#include <QVariantMap>

/// QIImagePicker provides a simple interface to access camera and camera roll via the UIImagePickerController
class QIImagePicker : public QObject
{
    Q_OBJECT
    Q_ENUMS(SourceType)
    Q_ENUMS(Status)
    Q_PROPERTY(SourceType sourceType READ sourceType WRITE setSourceType NOTIFY sourceTypeChanged)
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(bool busy READ busy WRITE setBusy NOTIFY busyChanged)
    Q_PROPERTY(QString mediaType READ mediaType WRITE setMediaType NOTIFY mediaTypeChanged)
    Q_PROPERTY(QString mediaUrl READ mediaUrl WRITE setMediaUrl NOTIFY mediaUrlChanged)
    Q_PROPERTY(QString referenceUrl READ referenceUrl WRITE setReferenceUrl NOTIFY referenceUrlChanged)

public:
    enum SourceType {
        PhotoLibrary,
        Camera,
        SavedPhotosAlbum
    };

    enum Status {
        Null, // Nothing loaded
        Running, // The view controller is running
        Ready, // The image is ready
        Saving // The image is saving
    };

    QIImagePicker(QObject* parent = nullptr);
    ~QIImagePicker();

    Q_INVOKABLE void show(bool animated = true);

    Q_INVOKABLE void close(bool animated = true);

    Q_INVOKABLE void save(QString fileName);

    /// Save the stored image to tmp file.
    Q_INVOKABLE void saveAsTemp();

    Q_INVOKABLE void clear();

    SourceType sourceType() const;
    void setSourceType(const SourceType &sourceType);

    QImage image() const;
    void setImage(const QImage &image);

    Status status() const;
    void setStatus(const Status &status);

    bool busy() const;
    void setBusy(bool busy);

    QString mediaType() const;
    void setMediaType(const QString &mediaType);

    QString mediaUrl() const;
    void setMediaUrl(const QString &mediaUrl);

    QString referenceUrl() const;
    void setReferenceUrl(const QString &referenceUrl);

signals:
    void sourceTypeChanged();
    void imageChanged();
    void statusChanged();
    void busyChanged();
    void referenceUrlChanged();
    void mediaTypeChanged();
    void mediaUrlChanged();

    void ready();
    void saved(QString url);

private:
    Q_INVOKABLE void onReceived(QString name,QVariantMap data);

    Q_INVOKABLE void endSave(QString fileName);

    SourceType m_sourceType;
    QImage m_image;
    Status m_status;

    QString m_mediaType;
    QString m_mediaUrl;
    QString m_referenceUrl;

    bool m_busy;
};

#endif // QIIMAGEPICKER_H
