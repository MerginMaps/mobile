/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "helper.h"
#include <QGuiApplication>
#include <QScreen>
#include <QFontDatabase>

Helper::Helper( QObject *parent ) : QObject{parent}
{

}

QString Helper::installFonts()
{
  QStringList fonts;
  fonts << ":/Inter-Regular.ttf"
        << ":/Inter-SemiBold.ttf";
  for ( const QString &font : fonts )
  {
    if ( QFontDatabase::addApplicationFont( font ) == -1 )
    {
      qDebug() << "!! Failed to load font" << font;
      exit( 3 );
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

bool Helper::isMobile()
{
#ifdef MOBILE_OS
  return true;
#endif
  return false;
}
