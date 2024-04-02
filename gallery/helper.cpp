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
  //
  // Keeping the previous implementation here in case we need it
  // See https://github.com/MerginMaps/mobile/pull/3200
  //

//  const QList<QScreen *> screens = QGuiApplication::screens();
//  if ( !screens.isEmpty() )
//  {
//    QScreen *screen = screens.at( 0 );
//    qreal realDpr = calculateScreenDpr();
//    return realDpr / screen->devicePixelRatio();
//  }

  return 1;
}

bool Helper::isMobile()
{
#ifdef MOBILE_OS
  return true;
#endif
  return false;
}

QString Helper::logText()
{
  QString ret(
    "<p class=\"even\">Input App: 2024.1.0 - macos (000000)</p><p class=\"odd\">Data Dir: /D/app-data-folder//qgis-data</p><p class=\"even\">System: macOS Ventura (13.2)</p><p class=\"odd\">"
    "Mergin URL: https://app.dev.merginmaps.com/</p><p class=\"even\">Mergin User: pptest6</p><p class=\"odd\">Mergin Data: 0.0/0.0 Bytes</p><p class=\"even\">Subscription plan:</p><p class=\"odd\">"
    "Subscription Status: free</p><p class=\"even\">------------------------------------------</p><p class=\"odd\">Screen Info:</p><p class=\"even\">screen resolution: 2560x1440 px</p><p class=\"odd\">"
    "screen DPI: 118x118</p><p class=\"even\">screen size: 551x310 mm</p><p class=\"odd\">reported device pixel ratio: 1</p><p class=\"even\">calculated device pixel ratio: 0.7375</p><p class=\"odd\">"
    "used dp scale: 0.7375</p><p class=\"even\">------------------------------------------</p><p class=\"odd\">Profiler Data:</p><p class=\"even\">"
    "QgsProject filename: /D/app-data-folder//qgis-data/projects/test_qrcode/test_qrcode.qgz</p><p class=\"odd\">List of QgsRuntimeProfiler events above 0.001 sec</p>"
    "<p class=\"even\">startup: total 1.126 sec</p><p class=\"odd\">> Setup tile download manager: 0.002 sec</p><p class=\"even\">> Load default style database: 0.365 sec</p>"
    "<p class=\"odd\">>> Load symbols: 0.112 sec</p><p class=\"even\">>>> cat trail: 0.003 sec</p><p class=\"odd\">>>> effect emboss: 0.004 sec</p><p class=\"even\">>>> hashed black X: 0.002 sec</p>"
    "<p class=\"odd\">>>> pattern circles: 0.002 sec</p><p class=\"even\">>> Load color ramps: 0.008 sec</p><p class=\"odd\">>> Load text formats: 0.224 sec</p><p class=\"even\">>>> Default: 0.224 sec</p>"
    "<p class=\"odd\">>> Load label settings: 0.007 sec</p><p class=\"even\">>>> watercourses: 0.004 sec</p><p class=\"odd\">>>> water bodies: 0.002 sec</p>"
    "<p class=\"even\">> Initialize data providers: 0.012 sec</p><p class=\"odd\">> Initialize authorities: 0.057 sec</p><p class=\"even\">> Initializing authentication manager: 0.005 sec</p>"
    "<p class=\"odd\">> Initialize ellipsoids: 0.065 sec</p><p class=\"even\">projectload: total 0.148 sec</p><p class=\"odd\">> Reading project file: 0.002 sec</p>"
    "<p class=\"even\">> Creating auxiliary storage: 0.017 sec</p><p class=\"odd\">> Reading properties: 0.004 sec</p><p class=\"even\">> Reading map layers: 0.033 sec</p><p class=\"odd\">>> OpenStreetMap: 0.012 sec</p>"
    "<p class=\"even\">>>> Load layer source: 0.011 sec</p><p class=\"odd\">>> test_qrcode: 0.020 sec</p><p class=\"even\">>>> Load layer source: 0.020 sec</p>"
    "<p class=\"odd\">>>>> Create ogr provider: 0.018 sec</p><p class=\"even\">> Updating CRS: 0.011 sec</p><p class=\"odd\">------------------------------------------</p>"
    "<p class=\"even\">1-01T14:33:47.010Z list invitations: Success</p><p class=\"odd\">2023-11-01T14:33:47.031Z list workspaces: Success</p>"
    "<p class=\"even\">2023-11-01T14:34:05.208Z list projects by name: Requesting: https://app.merginmaps.com//v1/project/by_names</p><p class=\"odd\">"
    "2023-11-01T14:34:05.210Z workspace info: Requesting workspace info: https://app.merginmaps.com/v1/workspace/0</p><p class=\"even\">2023-11-01T14:34:05.292Z workspace info: "
    "FAILED - Network API error: getWorkspaceInfo(): Error transferring https://app.merginmaps.com/v1/workspace/0 - server replied: NOT FOUND. Workspace not found</p><p class=\"odd\">"
    "2023-11-01T14:34:05.425Z list projects by name: Success - got 12 projects</p><p class=\"even\">2023-11-01T14:34:06.821Z list workspaces:"
    " Requesting: https://app.merginmaps.com//v1/workspaces</p><p class=\"odd\">2023-11-01T14:34:06.822Z list invitations: Requesting:"
    " https://app.merginmaps.com//v1/workspace/invitations</p><p class=\"even\">2023-11-01T14:34:06.899Z list invitations: Success</p>"
    "<p class=\"odd\">2023-11-01T14:34:06.926Z list workspaces: Success</p><p class=\"even\">2023-11-01T14:34:13.498Z list workspaces:"
    "Requesting: https://app.merginmaps.com//v1/workspaces</p><p class=\"odd\">2023-11-01T14:34:13.498Z list invitations: Requesting:"
    " https://app.merginmaps.com//v1/workspace/invitations</p><p class=\"even\">2023-11-01T14:34:13.566Z list invitations: Success</p>"
    "<p class=\"odd\">2023-11-01T14:34:13.599Z list workspaces: Success</p><p class=\"even\">2023-11-01T14:34:14.643Z list projects by"
    " name: Requesting: https://app.merginmaps.com//v1/project/by_names</p><p class=\"odd\">2023-11-01T14:34:14.645Z workspace info:"
    " Requesting workspace info: https://app.merginmaps.com/v1/workspace/193</p><p class=\"even\">2023-11-01T14:34:14.719Z workspace info:"
    " Success</p><p class=\"odd\">2023-11-01T14:34:14.720Z Service info: Requesting service info: https://app.merginmaps.com/v1/workspace/193/service</p>"
    "<p class=\"even\">2023-11-01T14:34:14.785Z Service info: FAILED - Network API error: getServiceInfo(): Error transferring https://app.merginmaps.com/v1/workspace/193/service"
    " - server replied: FORBIDDEN. You do not have owner permissions to workspace</p><p class=\"odd\">2023-11-01T14:34:14.851Z list projects by name: Success - got 12 projects</p>"
    "<p class=\"even\">2023-11-01T14:34:16.672Z list workspaces: Requesting: https://app.merginmaps.com//v1/workspaces</p><p class=\"odd\">2023-11-01T14:34:16.673Z list invitations: "
  );
  return ret;
}
