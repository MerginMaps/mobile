/***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/
#ifndef TESTSKETCHING_H
#define TESTSKETCHING_H

#include <QTemporaryDir>

#include "photosketchingcontroller.h"


class TestSketching : public QObject
{
    Q_OBJECT

  private slots:
    void initTestCase();
    void cleanupTestCase();
    void cleanup();

    void testNewSketch();
    void testUndoSketch();
    void testChangeColor();
    void testChangePhotoSize();
    void testSaveSketches();

  private:
    std::unique_ptr<PhotoSketchingController> mController = nullptr;
    const QTemporaryDir mTempDir;
};


#endif //TESTSKETCHING_H