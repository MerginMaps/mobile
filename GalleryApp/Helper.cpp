#include "Helper.h"
#include <QGuiApplication>
#include <QScreen>
#include <QFontDatabase>

Helper::Helper(QObject *parent) : QObject{parent}
{

}

QString Helper::installFonts()
{
  QStringList fonts;
  fonts << ":/fonts/Inter-Regular.ttf"
        << ":/fonts/Inter-SemiBold.ttf";
  for ( const QString &font : fonts )
  {
    if ( QFontDatabase::addApplicationFont( font ) == -1 ) {
      qDebug() << "!! Failed to load font" << font;
      exit(3);
    }
    else
      qDebug() << "Loaded font" << font;
  }
  return "Inter";
}

qreal Helper::calculateScreenDpr()
{
  const QList<QScreen *> screens = QGuiApplication::screens();

  if ( !screens.isEmpty() )
  {
    QScreen *screen = screens.at( 0 );
    double dpiX = screen->physicalDotsPerInchX();
    double dpiY = screen->physicalDotsPerInchY();

    qreal realDpi = dpiX < dpiY ? dpiX : dpiY;
    realDpi = realDpi * screen->devicePixelRatio();

    return realDpi / 160.;
  }

  return 1;
}

qreal Helper::calculateDpRatio()
{
  const QList<QScreen *> screens = QGuiApplication::screens();

  if ( !screens.isEmpty() )
  {
    QScreen *screen = screens.at( 0 );

    qreal realDpr = calculateScreenDpr();
    return realDpr / screen->devicePixelRatio();
  }

  return 1;
}
