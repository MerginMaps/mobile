/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTLOCALPROJECTSMANAGER_H
#define TESTLOCALPROJECTSMANAGER_H

#include <QObject>

class TestLocalProjectsManager : public QObject
{
    Q_OBJECT

  private slots:
    void init(); // will be called before each testfunction is executed.
    void cleanup(); // will be called after every testfunction.

    void testRenameSuccess();
    void testRenameEmptyName();
    void testRenameInvalidCharacters();
    void testRenameNameAlreadyTaken();
    void testRenameSameName();
    void testRenameUnknownProject();
    void testRenameDirectoryCollision();
    void testRenameTrimsWhitespace();

  private:
    QString mDataDir;
};

#endif // TESTLOCALPROJECTSMANAGER_H
