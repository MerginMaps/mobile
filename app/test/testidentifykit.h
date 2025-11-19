/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef TESTIDENTIFYKIT_H
#define TESTIDENTIFYKIT_H

#include <QObject>
#include <QtTest>

class TestIdentifyKit: public QObject
{
    Q_OBJECT
  private slots:
    void init() {} // will be called before each testfunction is executed.
    void cleanup() {} // will be called after every testfunction.

    void identifyOne(); // tests identifyOne function without given layer
    void identifyOneDefinedVector(); // tests identifyOne function with given layer
    void identifyInRadius();
};

#endif // TESTIDENTIFYKIT_H
