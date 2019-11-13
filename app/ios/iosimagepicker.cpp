#include "iosimagepicker.h"
#include "ioshandler.h"
#include "qdatetime.h"

#include <QDebug>
#include <QImageWriter>
#include <QUrl>

IOSImagePicker::IOSImagePicker(QObject *parent) : QObject(parent)
{

}

void IOSImagePicker::showImagePicker()
{
    //IOSHandler* instance = IOSHandler::instance();
    QObject::connect( mHandler, SIGNAL( imagePickerFinished( QString, QVariantMap ) ),
                      this, SLOT( onImagePickerFinished( QString, QVariantMap ) ) );
    mHandler->showImagePicker();
}

QString IOSImagePicker::targetDir() const
{
    return mTargetDir;
}

void IOSImagePicker::setTargetDir(const QString &targetDir)
{
    mTargetDir = targetDir;
    emit targetDirChanged();
}

void IOSImagePicker::onImagePickerFinished(QString name, QVariantMap data)
{
    qDebug() << "IOSImagePicker::onImageSaved!!!" << name;
    QObject::disconnect( mHandler, SIGNAL( imagePickerFinished( QString, QVariantMap ) ),
                      this, SLOT( onImagePickerFinished( QString, QVariantMap ) ) );

    QImage image = data["image"].value<QImage>();
    //      setImage( image );
    //      setMediaType( data["mediaType"].toString() );
    //      setMediaUrl( data["mediaUrl"].toString() );
    //      setReferenceUrl( data["referenceUrl"].toString() );

    QString absoluteImagePath = QString( "%1/%2.jpg" ).arg( mTargetDir, QDateTime::currentDateTime().toString( QStringLiteral( "yyMMdd-hhmmss" ) ) );

    image.save( absoluteImagePath );
    QImageWriter writer;
    writer.setFileName( absoluteImagePath );
    if ( !writer.write( image ) )
    {
        qWarning() << QString( "Failed to save %1 : %2" ).arg( absoluteImagePath ).arg( writer.errorString() );
    }

    QUrl url = QUrl::fromLocalFile( absoluteImagePath );
    emit imageSaved( url.toString() );
}

IOSHandler *IOSImagePicker::handler() const
{
    return mHandler;
}

void IOSImagePicker::setHandler(IOSHandler *handler)
{
    mHandler = handler;
    emit handlerChange();
}
