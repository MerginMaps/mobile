/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTUTILSFUNCTIONS_H
#define TESTUTILSFUNCTIONS_H

#include <QObject>
#include "inpututils.h"

class TestUtilsFunctions: public QObject
{
    Q_OBJECT
  public:
    TestUtilsFunctions() = default;
    ~TestUtilsFunctions() = default;
  private slots:
    void testFormatDuration();

  private:
    void testFormatDuration( const QDateTime &t0, qint64 diffSecs, const QString &expectedResult );
};

#endif // TESTUTILSFUNCTIONS_H
