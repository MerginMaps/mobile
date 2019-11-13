#include <QCoreApplication>
#include <QPointer>
#include <QtCore>
#include <QDebug>

#include <QImageWriter>
#include <QImageReader>

#include "ioshandler.h"

static IOSHandler *mInstance;

IOSHandler::IOSHandler( QObject *parent ) : QObject( parent )
{

}

IOSHandler *IOSHandler::instance()
{
  if ( !mInstance )
  {
    QCoreApplication *app = QCoreApplication::instance();
    mInstance = new IOSHandler( app );
  }
  return mInstance;
}

void IOSHandler::imagePickerFinished( bool successful, QVariantMap data )
{
  emit forwardedImagePickerFinished( successful, data );
}
