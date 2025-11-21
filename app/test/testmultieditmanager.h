/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef TESTMULTIEDITMANAGER_H
#define TESTMULTIEDITMANAGER_H

#include <QObject>
#include <QtTest/QTest>

#include "multieditmanager.h"


class TestMultiEditManager : public QObject
{
    Q_OBJECT

  public:
    TestMultiEditManager() = default;

  private:
    std::unique_ptr<QgsVectorLayer> mLayer;
    QVector<QgsFeature> mFeatures;
    std::unique_ptr<MultiEditManager> mManager;

  private slots:
    void init(); // will be called before each testfunction is executed.
    void cleanup(); // will be called after every testfunction.

    void testInitializeSelection();
    void testToggleSelectSameLayer();
    void testDeleteSelectedFeatures();

};

#endif // TESTMULTIEDITMANAGER_H
