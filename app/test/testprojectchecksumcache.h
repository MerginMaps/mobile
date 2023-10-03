/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTPROJECTCHECKSUMCACHE_H
#define TESTPROJECTCHECKSUMCACHE_H

#include <QObject>

class TestProjectChecksumCache : public QObject
{
    Q_OBJECT
  public:
    explicit TestProjectChecksumCache( );
    ~TestProjectChecksumCache();

  private slots:
    void init();
    void cleanup();

    void testFilesCheckum();
};

#endif // TESTPROJECTCHECKSUMCACHE_H
