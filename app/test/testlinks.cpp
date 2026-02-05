/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QDebug>
#include <QSignalSpy>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "inputhelp.h"
#include "testlinks.h"
#include "inpututils.h"
#include "testutils.h"

void UrlTester::processFinished()
{
  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );
  if ( r->error() == QNetworkReply::NoError )
  {
    qDebug() << "  URL " << mUrl << " ...OK";
    mResult = 0;
  }
  else
  {
    // for CI debugging
    qDebug() << "  URL " << mUrl << " ...ERROR. " << r->error();
    mResult = 1;
  }
  r->deleteLater();
  emit finished();
}

void UrlTester::open()
{
  QNetworkRequest req( mUrl );
  req.setRawHeader( "User-Agent", "InputApp tests" );
  QNetworkReply *reply = mManager.get( req );
  qDebug() << "Requesting " << mUrl;
  connect( reply, &QNetworkReply::finished, this, &UrlTester::processFinished );
}

TestLinks::TestLinks( MerginApi *api, InputUtils *utils ):
  mHelp( api )
{
}

void TestLinks::_run( const QString &url )
{
  UrlTester tester( url );
  QSignalSpy spy( &tester, &UrlTester::finished );
  tester.open();
  QVERIFY( spy.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );
  QCOMPARE( tester.result(), 0 );
}
