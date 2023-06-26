/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef TESTATTRIBUTECONTROLLER_H
#define TESTATTRIBUTECONTROLLER_H

#include <QObject>
#include <QtTest>

#include "inputconfig.h"

class TestAttributeController: public QObject
{
    Q_OBJECT
  private slots:
    void init(); // will be called before each testfunction is executed.
    void cleanup(); // will be called after every testfunction.

    void noFields();
    void twoFieldsAutoLayout();
    void twoTabsDragAndDropLayout();
    void twoGroupsDragAndDropLayout();
    void tabsAndFieldsMixed();
    void testValidationMessages();
    void testExpressions();
    void testRawValue();
    void testFieldsOutsideForm();
    void testPhotoRenaming();
};

#endif // TESTATTRIBUTECONTROLLER_H
