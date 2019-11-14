#include "iosimagepicker.h"
#include "qdatetime.h"

#include <QDebug>
#include <QImage>
#include <QImageWriter>
#include <QUrl>

IOSImagePicker::IOSImagePicker( QObject *parent ) : QObject( parent )
{
}

void IOSImagePicker::showImagePicker()
{
#ifdef Q_OS_IOS
  int sourceType = 0; // ImageGallery
  showImagePickerDirect( sourceType, this );
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

void IOSImagePicker::onImagePickerFinished( bool successful, const QVariantMap &data )
{
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
