/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inputhelp.h"
#include "merginuserauth.h"
#include "merginuserinfo.h"
#include "merginsubscriptioninfo.h"
#include "merginsubscriptionstatus.h"
#include "merginapi.h"
#include "inpututils.h"
#include "coreutils.h"

#include "inpututils.h"

#include <QNetworkReply>
#include <QSysInfo>

const QString inputHelpRoot = "https://help.inputapp.io";
const QString merginHelpRoot = "https://help.cloudmergin.com";
const QString reportLogUrl = QStringLiteral( "https://g4pfq226j0.execute-api.eu-west-1.amazonaws.com/mergin_client_log_submit" );
const QString helpDeskMail = QStringLiteral( "info@lutraconsulting.co.uk" );

InputHelp::InputHelp( MerginApi *merginApi, InputUtils *utils ):
  mMerginApi( merginApi ),
  mInputUtils( utils )
{
  emit linkChanged();
}

QString InputHelp::helpRootLink() const
{
  return inputHelpRoot;
}

QString InputHelp::privacyPolicyLink() const
{
  return inputHelpRoot + "/privacy";
}

QString InputHelp::merginSubscriptionDetailsLink() const
{
  return merginHelpRoot + "/subscriptions";
}

QString InputHelp::howToEnableDigitizingLink() const
{
  return inputHelpRoot + "/howto/enable_digitizing";
}

QString InputHelp::howToEnableBrowsingDataLink() const
{
  return inputHelpRoot + "/howto/enable_browsing";
}

QString InputHelp::howToSetupThemesLink() const
{
  return inputHelpRoot + "/howto/setup_themes";
}

QString InputHelp::howToCreateNewProjectLink() const
{
  return inputHelpRoot + "/howto/project_config";
}

QString InputHelp::howToDownloadProjectLink() const
{
  return inputHelpRoot + "/howto/data_sync";
}

QString InputHelp::howToSetupProj() const
{
  return inputHelpRoot + "/howto/proj";
}

QString InputHelp::merginTermsLink() const
{
  return QStringLiteral( "https://public.cloudmergin.com/assets/tos.html" );
}

bool InputHelp::submitReportPending() const
{
  return mSubmitReportPending;
}

QString InputHelp::fullLog( bool isHtml )
{
  qint64 limit = 500000;
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
  retLines.push_back( QStringLiteral( "Input App: %1 - %2" ).arg( CoreUtils::appVersion() ).arg( InputUtils::appPlatform() ) );
  retLines.push_back( QStringLiteral( "System: %1" ).arg( QSysInfo::prettyProductName() ) );
  retLines.push_back( QStringLiteral( "Mergin URL: %1" ).arg( mMerginApi->apiRoot() ) );
  retLines.push_back( QStringLiteral( "Mergin User: %1" ).arg( mMerginApi->userAuth()->username() ) );
  if ( !mMerginApi->userInfo()->email().isEmpty() )
  {
    retLines.push_back( QStringLiteral( "Mergin Data: %1/%2 Bytes" )
                        .arg( InputUtils::bytesToHumanSize( mMerginApi->userInfo()->diskUsage() ) )
                        .arg( InputUtils::bytesToHumanSize( mMerginApi->userInfo()->storageLimit() ) ) );
    retLines.push_back( QStringLiteral( "Subscription plan: %1" ).arg( mMerginApi->subscriptionInfo()->planAlias() ) );
    retLines.push_back( QStringLiteral( "Subscription Status: %1" ).arg( MerginSubscriptionStatus::toString( static_cast<MerginSubscriptionStatus::SubscriptionStatus>( mMerginApi->subscriptionInfo()->subscriptionStatus() ) ) ) );
  }
  else
  {
    retLines.push_back( QStringLiteral( "%1Mergin User Profile not available. To include it, open you Profile Page in InputApp%2" ).arg( isHtml ? "<b>" : "" ).arg( isHtml ? "</b>" : "" ) );
  }
  retLines.push_back( QStringLiteral( "Screen Info:" ) );
  retLines.append( InputUtils().dumpScreenInfo().split( "\n" ).toVector() );
  retLines.push_back( QStringLiteral( "------------------------------------------" ) );
  return retLines;
}


void InputHelp::submitReport()
{
  // There is a limit of 1MB on the remote service, send less, let say half of that
  QString log = fullLog( false );
  QByteArray logArr = log.toUtf8();
  QString app = QStringLiteral( "input-%1-%2" ).arg( InputUtils::appPlatform() ).arg( CoreUtils::appVersion() );
  QString username = mMerginApi->userAuth()->username().toHtmlEscaped();
  if ( username.isEmpty() )
    username = "unknown";
  QString params = QStringLiteral( "?app=%1&username=%2" ).arg( app ).arg( username );
  QNetworkRequest req( reportLogUrl + params );
  req.setRawHeader( "User-Agent", "InputApp" );
  req.setRawHeader( "Content-Type", "text/plain" );
  QNetworkReply *reply = mManager.post( req, logArr );
  qDebug() << "Report to " << reportLogUrl << endl;

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
    emit mInputUtils->showNotification( tr( "Report submitted.%1Please contact us on%1%2" ).arg( "<br />" ).arg( helpDeskMail ) );
  }
  else
  {
    CoreUtils::log( "submit report", QStringLiteral( "FAILED - %1" ).arg( r->errorString() ) );
    emit mInputUtils->showNotification( tr( "Failed to submit report.%1Please check your internet connection." ).arg( "<br>" ) );
  }
}
