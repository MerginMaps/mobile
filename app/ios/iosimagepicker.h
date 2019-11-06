#ifndef QIIMAGEPICKER_H
#define QIIMAGEPICKER_H

#include <QObject>
#include <QImage>

#include <QVariant>
#include <QString>
#include <QMap>
#include <QVariantMap>

/// IOSImagePicker provides a simple interface to access camera and camera roll via the UIImagePickerController
class IOSImagePicker : public QObject
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
    Q_PROPERTY(QString targetPath READ targetPath WRITE setTargetPath NOTIFY targetPathChanged)

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

    IOSImagePicker(QObject* parent = nullptr);
    ~IOSImagePicker();

    Q_INVOKABLE void show(bool animated = true);

    Q_INVOKABLE void close(bool animated = true);

    Q_INVOKABLE void save(QString fileName);

    Q_INVOKABLE void saveImage();

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

    QString targetPath() const;
    void setTargetPath(const QString &targetPath);

signals:
    void sourceTypeChanged();
    void imageChanged();
    void statusChanged();
    void busyChanged();
    void referenceUrlChanged();
    void mediaTypeChanged();
    void mediaUrlChanged();
    void targetPathChanged();

    void ready();
    void saved(QString url);

private:
    Q_INVOKABLE void onReceived(QString name,QVariantMap data);

    Q_INVOKABLE void endSave(QString fileName);

    SourceType mSourceType = PhotoLibrary;
    QImage mImage;
    Status mStatus;

    QString mMediaType;
    QString mMediaUrl;
    QString mReferenceUrl;

    QString mTargetPath;

    bool mBusy;
};

#endif // QIIMAGEPICKER_H
