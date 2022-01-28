/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QObject>
#include <QtTest>

#ifndef TESTPOSITIONKIT_H
#define TESTPOSITIONKIT_H

#include "position/positionkit.h"

class TestPositionKit: public QObject
{
    Q_OBJECT
  private slots:
    void init(); // will be called before each testfunction is executed.
    void cleanup() {} // will be called after every testfunction.

    void simulatedPosition();
    void testBluetoothProviderConnection();
    void testBluetoothProviderPosition();
    void testSettingsProvidersKeys();
    void testMapPosition();
    void testPositionDirection();

  private:
    PositionKit positionKit;
};

#endif // TESTPOSITIONKIT_H
