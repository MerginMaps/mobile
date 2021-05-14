/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTATTRIBUTEPREVIEWCONTROLLER_H
#define TESTATTRIBUTEPREVIEWCONTROLLER_H

#include <QTest>
#include <QObject>

class TestAttributePreviewController: public QObject
{
    Q_OBJECT
  public:
    explicit TestAttributePreviewController( );
    ~TestAttributePreviewController() override;

  private slots:

    // global init + cleanup functions
    void initTestCase();
    void cleanupTestCase();

    void testPreviewForms();

  private:
};

#endif // TESTATTRIBUTEPREVIEWCONTROLLER_H
