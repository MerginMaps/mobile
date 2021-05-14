/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <QString>
#include <qtestcase.h>

namespace TestUtils
{
  const int SHORT_REPLY = 5000;
  const int LONG_REPLY = 70000;

  void mergin_auth( QString &apiRoot, QString &username, QString &password );
  QString testDataDir();
}

#define COMPARENEAR(actual, expected, epsilon) \
  do {\
    if (!QTest::compare_helper((qAbs(actual - expected) <= epsilon), \
    QString{"Compared values are not the same in respect to epsilon %1"} \
  .arg(epsilon).toLocal8Bit().constData(), \
  QTest::toString(actual), \
  QTest::toString(expected), \
  #actual, #expected, __FILE__, __LINE__)) \
    return;\
  } while (false)

#endif // TESTUTILS_H
