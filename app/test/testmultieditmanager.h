/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QObject>

#ifndef TESTMULTIEDITMANAGER_H
#define TESTMULTIEDITMANAGER_H

class TestMultiEditManager : public QObject {
    Q_OBJECT
  private slots:
    void check();
};

#endif // TESTMULTIEDITMANAGER_H