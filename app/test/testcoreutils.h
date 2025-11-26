/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTCOREUTILS_H
#define TESTCOREUTILS_H

#include <QObject>

class TestCoreUtils : public QObject
{
    Q_OBJECT

  private slots:
    void init(); // will be called before each testfunction is executed.
    void cleanup(); // will be called after every testfunction.

    void testConflictFileNames();
    void testFindUniquePath();
    void testHasProjectFileExtension();
    void testNameValidation();
    void testNameAbbr();
    void testReplaceValueInJson();
};

#endif // TESTCOREUTILS_H
