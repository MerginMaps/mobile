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

class IOSImagePickerSaver : public QRunnable
{
  public:
    QPointer<IOSImagePicker> owner;
    QImage image;
    QString absoluteImagePath;

    /// <#Description#>
    void run()
    {
      if ( absoluteImagePath.isNull() )
      {
        absoluteImagePath = QString( "%1/%2.jpg" ).arg( owner->targetPath(), QDateTime::currentDateTime().toString( QStringLiteral( "yyMMdd-hhmmss" ) ) );
      }

      image.save( absoluteImagePath );
      QImageWriter writer;
      writer.setFileName( absoluteImagePath );
      if ( !writer.write( image ) )
      {
        qWarning() << QString( "Failed to save %1 : %2" ).arg( absoluteImagePath ).arg( writer.errorString() );
      }

      if ( !owner.isNull() )
      {
        QMetaObject::invokeMethod( owner.data(), "endSave", Qt::QueuedConnection,
                                   Q_ARG( QString, absoluteImagePath ) );
      }
    }
};

IOSImagePicker::IOSImagePicker( QObject *parent ) : QObject( parent )
{
  mSourceType = PhotoLibrary;
  mStatus = Null;
  mBusy = false;
}

IOSImagePicker::~IOSImagePicker()
{

}

void IOSImagePicker::show( bool animated )
{
  if ( mStatus == Running || mStatus == Saving )
  {
    return;
  }

#ifdef Q_OS_IOS
  QISystemDispatcher *system = QISystemDispatcher::instance();

  QVariantMap data;
  data["sourceType"] = mSourceType;
  data["animated"] = animated;

  connect( system, SIGNAL( dispatched( QString, QVariantMap ) ),
           this, SLOT( onReceived( QString, QVariantMap ) ) );

  bool res = system->dispatch( "imagePickerControllerPresent", data );

  if ( res )
  {
    setStatus( Running );
  }
#endif
}

void IOSImagePicker::close( bool animated )
{
  QISystemDispatcher *system = QISystemDispatcher::instance();
  QVariantMap data;
  data["animated"] = animated;

  system->dispatch( "imagePickerControllerDismiss", data );
}

void IOSImagePicker::save( QString fileName )
{
  IOSImagePickerSaver *saver = new IOSImagePickerSaver();
  saver->setAutoDelete( true );
  saver->owner = this;
  saver->absoluteImagePath = fileName;
  saver->image = mImage;

  QThreadPool::globalInstance()->start( saver );
}

void IOSImagePicker::saveImage()
{
  save( QString() );
}

void IOSImagePicker::clear()
{
  setImage( QImage() );
  emit imageChanged();
  setStatus( Null );
}

IOSImagePicker::SourceType IOSImagePicker::sourceType() const
{
  return mSourceType;
}

void IOSImagePicker::setSourceType( const SourceType &sourceType )
{
  mSourceType = sourceType;
  emit sourceTypeChanged();
}

QImage IOSImagePicker::image() const
{
  return mImage;
}

void IOSImagePicker::setImage( const QImage &image )
{
  mImage = image;
  if ( true )
  {
    QString mFilename;
    mImage.save( mFilename, "PNG" );
  }

  emit imageChanged();
}

IOSImagePicker::Status IOSImagePicker::status() const
{
  return mStatus;
}

void IOSImagePicker::setStatus( const Status &status )
{
  if ( mStatus == status )
    return;
  mStatus = status;
  emit statusChanged();
}

void IOSImagePicker::onReceived( QString name, QVariantMap data )
{
  QISystemDispatcher *system = QISystemDispatcher::instance();

  if ( name == "imagePickerControllerDidCancel" )
  {
    setStatus( Null );
    system->disconnect( this );
    close();
  }

  if ( name != "imagePickerControllerDisFinishPickingMetaWithInfo" )
    return;

  system->disconnect( this );
  QImage image = data["image"].value<QImage>();
  setImage( image );
  setMediaType( data["mediaType"].toString() );
  setMediaUrl( data["mediaUrl"].toString() );
  setReferenceUrl( data["referenceUrl"].toString() );

  setStatus( Ready );
  emit ready();
}

void IOSImagePicker::endSave( QString fileName )
{
  QUrl url = QUrl::fromLocalFile( fileName );

  emit saved( url.toString() );

  if ( mImage.isNull() )
  {
    setStatus( Null );
  }
  else
  {
    setStatus( Ready );
  }
}

QString IOSImagePicker::targetPath() const
{
  return mTargetPath;
}

void IOSImagePicker::setTargetPath( const QString &targetPath )
{
  mTargetPath = targetPath;
  emit targetPathChanged();
}
QString IOSImagePicker::referenceUrl() const
{
  return mReferenceUrl;
}

void IOSImagePicker::setReferenceUrl( const QString &referenceUrl )
{
  mReferenceUrl = referenceUrl;
  emit referenceUrlChanged();
}

QString IOSImagePicker::mediaUrl() const
{
  return mMediaUrl;
}

void IOSImagePicker::setMediaUrl( const QString &mediaUrl )
{
  mMediaUrl = mediaUrl;
  emit mediaUrlChanged();
}

QString IOSImagePicker::mediaType() const
{
  return mMediaType;
}

void IOSImagePicker::setMediaType( const QString &mediaType )
{
  mMediaType = mediaType;
  emit mediaTypeChanged();
}

bool IOSImagePicker::busy() const
{
  return mBusy;
}

void IOSImagePicker::setBusy( bool busy )
{
  if ( mBusy == busy )
    return;

  mBusy = busy;
  QISystemDispatcher *system = QISystemDispatcher::instance();

  QVariantMap data;
  data["active"] = mBusy;

  system->dispatch( "imagePickerControllerSetIndicator", data );
  emit busyChanged();
}




