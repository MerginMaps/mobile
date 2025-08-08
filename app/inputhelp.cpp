/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inputhelp.h"
#include "merginuserinfo.h"
#include "merginsubscriptioninfo.h"
#include "merginsubscriptionstatus.h"
#include "merginapi.h"
#include "inpututils.h"
#include "coreutils.h"

#include "inpututils.h"

#include <QNetworkReply>
#include <QSysInfo>

const QString helpRoot = QStringLiteral( "https://merginmaps.com/docs" );
const QString helpDeskMail = QStringLiteral( "support@merginmaps.com" );
const QString mmWeb = QStringLiteral( "https://merginmaps.com" );
const QString changelogRss = QStringLiteral( "https://wishlist.merginmaps.com/rss/changelog.xml" );

const QString utmTagHelp = QStringLiteral( "?utm_source=input-help&utm_medium=help&utm_campaign=input" );
const QString utmTagSubscription = QStringLiteral( "?utm_source=input-subs&utm_medium=subs&utm_campaign=input" );
const QString utmTagOther = QStringLiteral( "?utm_source=input-other&utm_medium=other&utm_campaign=input" );
const QString utmTagAttention = QStringLiteral( "?utm_source=input-app&utm_medium=attention-required" );


InputHelp::InputHelp( MerginApi *merginApi ):
  mMerginApi( merginApi )
{
  connect( mMerginApi, &MerginApi::apiRootChanged, this, &InputHelp::merginLinkChanged );

  emit linkChanged();
}

QString InputHelp::helpRootLink() const
{
  return helpRoot + "/" + utmTagHelp;
}

QString InputHelp::mmWebLink() const
{
  return mmWeb + "/" +  utmTagOther;
}

QString InputHelp::merginLinkHelper( const QString &subpath, const QString &utmTag ) const
{
  QString activeWorkspacePathPart;

  if ( mMerginApi && mMerginApi->apiSupportsWorkspaces() )
  {
    int activeWS = mMerginApi->userInfo()->activeWorkspaceId();
    if ( activeWS >= 0 )
    {
      activeWorkspacePathPart = QStringLiteral( "?workspace=%1" ).arg( activeWS );
    }
  }

  if ( mMerginApi && mMerginApi->apiRoot() != MerginApi::defaultApiRoot() )
  {
    return mMerginApi->apiRoot() + "/" + subpath + activeWorkspacePathPart;
  }

  // Let's include UTM tags for production server
  QString queryParams;

  if ( !activeWorkspacePathPart.isEmpty() )
  {
    queryParams = activeWorkspacePathPart;

    // URL can not have two question marks, merge the tags with &
    QString utms( utmTag );
    utms.replace( "?", "&" );
    queryParams += utms;
  }
  else
  {
    queryParams = utmTag;
  }

  return MerginApi::defaultApiRoot() + "/" + subpath + queryParams;
}

QString InputHelp::merginDashboardLink() const
{
  return merginLinkHelper( "dashboard", utmTagAttention );
}

QString InputHelp::merginSubscriptionLink() const
{
  if ( mMerginApi && mMerginApi->apiSupportsSubscriptions() )
  {
    return merginLinkHelper( "subscription", utmTagSubscription );
  }
  else
  {
    return merginDashboardLink();
  }
}

QString InputHelp::privacyPolicyLink() const
{
  return mmWeb + "/privacy-policy" + utmTagOther;
}

QString InputHelp::merginSubscriptionDetailsLink() const
{
  return helpRoot + "/setup/subscriptions/" + utmTagSubscription;
}

QString InputHelp::howToEnableDigitizingLink() const
{
  return helpRoot + "/gis/enable_digitising/" + utmTagHelp;
}

QString InputHelp::howToEnableBrowsingDataLink() const
{
  return helpRoot + "/gis/enable_browsing/" + utmTagHelp;
}

QString InputHelp::howToSetupThemesLink() const
{
  return helpRoot + "/gis/setup_themes/" + utmTagHelp;
}

QString InputHelp::howToSetupProj() const
{
  return helpRoot + "/gis/proj/" + utmTagHelp;
}

QString InputHelp::gpsAccuracyHelpLink() const
{
  return helpRoot + "/field/gps_accuracy/" + utmTagHelp; // NOTE: keeping here for future use
}

QString InputHelp::howToConnectGPSLink() const
{
  return helpRoot + "/field/external_gps/" + utmTagHelp;
}

QString InputHelp::merginTermsLink() const
{
  return mmWeb + "/terms-of-service" + utmTagOther;
}

QString InputHelp::projectLoadingErrorHelpLink() const
{
  return helpRoot + "/field/broken-project/" + utmTagHelp;
}

QString InputHelp::whatsNewPostLink() const
{
  return mmWeb + "/blog/introducing-workspaces-simplified-collaboration" + utmTagOther;
}

QString InputHelp::changelogLink()
{
  return changelogRss;
}

QString InputHelp::helpdeskMail()
{
  return helpDeskMail;
}

QString InputHelp::migrationGuides() const
{
  return helpRoot + "/dev/ce-migration/" + utmTagHelp;
}

bool InputHelp::submitReportPending() const
{
  return mSubmitReportPending;
}

QString InputHelp::fullLog( bool isHtml )
{
  qint64 limit = 5 * 1024 * 1024; // 5MB
  QVector<QString> retLines = logHeader( isHtml );

  QFile file( CoreUtils::logFilename() );
  if ( file.open( QIODevice::ReadOnly ) )
  {
    qint64 fileSize = file.size();
    if ( fileSize > limit )
      file.seek( file.size() - limit );

    QString line = file.readLine();
    while ( !line.isNull() )
    {
      retLines.push_back( line );
      line = file.readLine();
    }

    file.close();
  }
  else
  {
    retLines.push_back( QString( "Unable to open log file %1" ).arg( CoreUtils::logFilename() ) );
  }

  QString ret;
  int i = 0;
  for ( const QString &str : retLines )
  {
    ++i;
    if ( isHtml )
      ret += QStringLiteral( "<p class=\"%1\">" ).arg( i % 2 == 0 ? "odd" : "even" ) + str.trimmed() + "</p>";
    else
      ret += str.trimmed() + "\n";
  }

  return ret;
}

QVector<QString> InputHelp::logHeader( bool isHtml )
{
  QVector<QString> retLines;
  retLines.push_back( QStringLiteral( "Input App: %1 - %2 (%3)" ).arg( CoreUtils::appVersion() ).arg( InputUtils::appPlatform() ).arg( CoreUtils::appVersionCode() ) );
  retLines.push_back( QStringLiteral( "Device UUID: %1" ).arg( CoreUtils::deviceUuid() ) );
  retLines.push_back( QStringLiteral( "Data Dir: %1" ).arg( InputUtils::appDataDir() ) );
  retLines.push_back( QStringLiteral( "System: %1" ).arg( QSysInfo::prettyProductName() ) );
  retLines.push_back( QStringLiteral( "CPU Architecture: %1" ).arg( QSysInfo::currentCpuArchitecture() ) );
  retLines.push_back( QStringLiteral( "Device Model: %1" ).arg( InputUtils::getDeviceModel() ) );
  retLines.push_back( QStringLiteral( "Device Manufacturer: %1" ).arg( InputUtils::getManufacturer() ) );
  retLines.push_back( QStringLiteral( "Mergin URL: %1" ).arg( mMerginApi->apiRoot() ) );
  retLines.push_back( QStringLiteral( "Mergin User: %1" ).arg( mMerginApi->userInfo()->username() ) );
  if ( !mMerginApi->userInfo()->email().isEmpty() )
  {
    retLines.push_back( QStringLiteral( "Mergin Data: %1/%2 Bytes" )
                        .arg( InputUtils::bytesToHumanSize( mMerginApi->workspaceInfo()->diskUsage() ) )
                        .arg( InputUtils::bytesToHumanSize( mMerginApi->workspaceInfo()->storageLimit() ) ) );
    retLines.push_back( QStringLiteral( "Workspace Name: %1" ).arg( mMerginApi->userInfo()->activeWorkspaceName() ) );
    retLines.push_back( QStringLiteral( "Workspace ID: %1" ).arg( mMerginApi->userInfo()->activeWorkspaceId() ) );
  }
  else
  {
    retLines.push_back( QStringLiteral( "%1Mergin User Profile not available. To include it, open you Profile Page in InputApp%2" ).arg( isHtml ? "<b>" : "" ).arg( isHtml ? "</b>" : "" ) );
  }
  retLines.push_back( QStringLiteral( "------------------------------------------" ) );
  retLines.push_back( QStringLiteral( "Screen Info:" ) );
  retLines.append( InputUtils().dumpScreenInfo().split( "\n" ).toVector() );
  retLines.push_back( QStringLiteral( "------------------------------------------" ) );
  retLines.push_back( QStringLiteral( "Profiler Data:" ) );
  retLines.append( InputUtils().qgisProfilerLog() );
  retLines.push_back( QStringLiteral( "------------------------------------------" ) );

  return retLines;
}


void InputHelp::submitReport()
{
  // There is a limit of 6MB on the remote service, send less, let say 5MB
  QString log = fullLog( false );
  QByteArray logArr = log.toUtf8();
  QString app = QStringLiteral( "input-%1-%2" ).arg( InputUtils::appPlatform() ).arg( CoreUtils::appVersion() );
  QString username = mMerginApi->userInfo()->username().toHtmlEscaped();
  if ( username.isEmpty() )
    username = "unknown";
  QString params = QStringLiteral( "?app=%1&username=%2" ).arg( app ).arg( username );
  QNetworkRequest req( mMerginApi->serverDiagnosticLogsUrl() + params );
  req.setRawHeader( "User-Agent", "InputApp" );
  req.setRawHeader( "Content-Type", "text/plain" );
  QNetworkReply *reply = mManager.post( req, logArr );

  mSubmitReportPending = true;
  emit submitReportPendingChanged();
  connect( reply, &QNetworkReply::finished, this, &InputHelp::onSubmitReportReplyFinished );
}

void InputHelp::onSubmitReportReplyFinished()
{
  mSubmitReportPending = false;
  emit submitReportPendingChanged();

  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    CoreUtils::log( "submit report", "Report submitted!" );
    emit submitReportSuccessful();
  }
  else
  {
    CoreUtils::log( "submit report", QStringLiteral( "FAILED - %1" ).arg( r->errorString() ) );
    emit submitReportFailed();
  }
}
