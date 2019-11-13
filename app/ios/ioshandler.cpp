#include <QCoreApplication>
#include <QPointer>
#include <QtCore>
#include <QDebug>

#include <QImageWriter>
#include <QImageReader>

#include "ioshandler.h"

static IOSHandler* mInstance;

IOSHandler::IOSHandler(QObject *parent) : QObject(parent)
{

}

IOSHandler *IOSHandler::instance()
{
    if (!mInstance) {
        QCoreApplication* app = QCoreApplication::instance();
        mInstance = new IOSHandler(app);
    }
    return mInstance;
}

//QString IOSHandler::targetDir()
//{
//    return mTargetDir;
//}

//void IOSHandler::setTargetDir(const QString &targetDir)
//{
//    mTargetDir = targetDir;
//    //emit targetDirChanged();
//}

void IOSHandler::imagePickerChoosen(QString name , QVariantMap data)
{
    qDebug() << "IOSHANDLER::imagePickerChoosen" << name;

//    QImage image = data["image"].value<QImage>();
//    QString mediaType = data["mediaType"].toString();
//    QString setMediaUrl = data["mediaUrl"].toString();
//    QString referenceUrl = data["referenceUrl"].toString();
//    QString targetPath;

    //QString absoluteImagePath = QString( "%1/%2.jpg" ).arg( mTargetDir, QDateTime::currentDateTime().toString( QStringLiteral( "yyMMdd-hhmmss" ) ) );


//    image.save( absoluteImagePath );
//    QImageWriter writer;
//    writer.setFileName( absoluteImagePath );
//    if ( !writer.write( image ) )
//    {
//        qWarning() << QString( "Failed to save %1 : %2" ).arg( absoluteImagePath ).arg( writer.errorString() );
//    }

//    // TODO post process
//    QUrl url = QUrl::fromLocalFile( absoluteImagePath );

//    IOSHandler* handler = IOSHandler::instance();
//    // TODO invokeMethod and catch it with ImagePicker or some handler
//    QMetaObject::invokeMethod(handler,"imagePickerChoosen",Qt::DirectConnection,
//                                  Q_ARG(QString , "imagePickerControllerDidFinishPickingMediaWithInfo"),
//                                  Q_ARG(QVariantMap,data));

    emit imagePickerFinished( name, data );
}
