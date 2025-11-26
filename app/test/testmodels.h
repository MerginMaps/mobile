/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTMODELS_H
#define TESTMODELS_H

#include <QObject>

class TestModels : public QObject
{
    Q_OBJECT

  private slots:
    void init(); // will be called before each testfunction is executed.
    void cleanup(); // will be called after every testfunction.

    void testStaticFeaturesModel();
    void testLayerFeaturesModel();
    void testLayerFeaturesModelSorted();
    void testValueRelationFeaturesModel();
    void testProjectsModel();
    void testProjectsProxyModel();

};

#endif // TESTMODELS_H
