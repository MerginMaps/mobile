/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTFORMS_H
#define TESTFORMS_H

#include <QTest>
#include <QObject>

class TestForms: public QObject
{
    Q_OBJECT
  public:
    explicit TestForms( );
    ~TestForms() override;

  private slots:

    // global init + cleanup functions
    void initTestCase();
    void cleanupTestCase();

    void testPreviewForms();

  private:
};

#endif // TESTFORMS_H
