#include "iosimagepicker.h"
#include "ioshandler.h"
#include "qdatetime.h"

#include <QDebug>
#include <QImageWriter>
#include <QUrl>

IOSImagePicker::IOSImagePicker( QObject *parent ) : QObject( parent )
{
  mHandler = IOSHandler::instance();
}

void IOSImagePicker::showImagePicker( int sourceType )
{
#ifdef Q_OS_IOS
  QObject::connect( mHandler, SIGNAL( forwardedImagePickerFinished( bool, QVariantMap ) ),
                    this, SLOT( onImagePickerFinished( bool, QVariantMap ) ) );
  mHandler->showImagePicker( sourceType, mHandler );
#endif
}

QString IOSImagePicker::targetDir() const
{
  return mTargetDir;
}

void IOSImagePicker::setTargetDir( const QString &targetDir )
{
  mTargetDir = targetDir;
  emit targetDirChanged();
}

void IOSImagePicker::onImagePickerFinished( bool successful, QVariantMap data )
{

  QObject::disconnect( mHandler, SIGNAL( forwardedImagePickerFinished( bool, QVariantMap ) ),
                       this, SLOT( onImagePickerFinished( bool, QVariantMap ) ) );

  if ( successful )
  {
    QImage image = data["image"].value<QImage>();
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
}
