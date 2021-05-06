/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testutilsfunctions.h"

#include <QtTest/QtTest>
#include <QtCore/QObject>

const int DAY_IN_SECS = 60 * 60 * 24;
const int MONTH_IN_SECS = 60 * 60 * 24 * 31;

void TestUtilsFunctions::testFormatDuration()
{
  QDateTime t0 = QDateTime::currentDateTime();

  testFormatDuration( t0, -1, QStringLiteral( "Invalid datetime" ) );
  testFormatDuration( t0, 0, QStringLiteral( "just now" ) );
  testFormatDuration( t0, 1, QStringLiteral( "just now" ) );
  testFormatDuration( t0, 60, QStringLiteral( "1 minute ago" ) );
  testFormatDuration( t0, 2 * 60, QStringLiteral( "2 minutes ago" ) );
  testFormatDuration( t0, 1 * 60 * 60, QStringLiteral( "1 hour ago" ) );
  testFormatDuration( t0, 2 * 60 * 60, QStringLiteral( "2 hours ago" ) );
  testFormatDuration( t0, 1 * DAY_IN_SECS, QStringLiteral( "1 day ago" ) );
  testFormatDuration( t0, 2 * DAY_IN_SECS, QStringLiteral( "2 days ago" ) );
  testFormatDuration( t0, 7 * DAY_IN_SECS, QStringLiteral( "1 week ago" ) );
  testFormatDuration( t0, 14 * DAY_IN_SECS, QStringLiteral( "2 weeks ago" ) );
  testFormatDuration( t0, MONTH_IN_SECS, QStringLiteral( "1 month ago" ) );
  testFormatDuration( t0, 2 * MONTH_IN_SECS, QStringLiteral( "2 months ago" ) );
  testFormatDuration( t0, 12 * MONTH_IN_SECS, QStringLiteral( "1 year ago" ) );
  testFormatDuration( t0, 24 * MONTH_IN_SECS, QStringLiteral( "2 years ago" ) );
}

void TestUtilsFunctions::testFormatDuration( const QDateTime &t0, qint64 diffSecs, const QString &expectedResult )
{
  QDateTime t1 = t0.addSecs( diffSecs );
  QString str_t1 = InputUtils::formatDateTimeDiff( t0, t1 );
  QCOMPARE( str_t1, expectedResult );
}
