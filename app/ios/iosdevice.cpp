#include <QPointer>
#include <QCoreApplication>
#include "iosdevice.h"

static QPointer<QIDevice> m_instance;

QIDevice::QIDevice( QObject *parent ) : QObject( parent )
{
  m_screenFillStatusBar = false;
  m_screenWidth = -1;
  m_screenHeight = -1;

  QVariantMap data = fetch();

  QStringList properties;
  properties << "identifierForVendor";

  for ( int i = 0 ; i < properties.count() ; i++ )
  {
    QString property = properties.at( i );
    if ( data.contains( property ) )
    {
      setProperty( property.toLocal8Bit().constData(),
                   data[property] );
    }
  }


}

QString QIDevice::identifierForVendor() const
{
  return m_identifierForVendor;
}

void QIDevice::setIdentifierForVendor( const QString &identifierForVendor )
{
  m_identifierForVendor = identifierForVendor;
  emit identifierForVendorChanged();
}

#ifndef Q_OS_IOS
QVariantMap QIDevice::fetch() const
{
  QVariantMap data;
  return data;
}
#endif


QIDevice::~QIDevice()
{

}

bool QIDevice::screenFillStatusBar() const
{
  return m_screenFillStatusBar;
}

void QIDevice::setScreenFillStatusBar( bool screenFillStatusBar )
{
  m_screenFillStatusBar = screenFillStatusBar;
  emit screenFillStatusBarChanged();
}

int QIDevice::screenWidth() const
{
  return m_screenWidth;
}

void QIDevice::setScreenWidth( int screenWidth )
{
  m_screenWidth = screenWidth;
  emit screenHeightChanged();
}

int QIDevice::screenHeight() const
{
  return m_screenHeight;
}

void QIDevice::setScreenHeight( int screenHeight )
{
  m_screenHeight = screenHeight;
  emit screenHeightChanged();
}

QIDevice *QIDevice::instance()
{
  if ( m_instance.isNull() )
  {
    QCoreApplication *app = QCoreApplication::instance();
    m_instance = new QIDevice( app );
  }

  return m_instance;
}



