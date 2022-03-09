/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SYNCHRONIZATIONCONTROLLER_H
#define SYNCHRONIZATIONCONTROLLER_H

#include <QObject>
#include <qglobal.h>

#include <project.h>

class SynchronizationController : public QObject
{
    Q_OBJECT
  public:
    explicit SynchronizationController( QObject *parent = nullptr );

    void syncProject( const QString &projectId );

  public slots:
    void activeProjectChanged( LocalProject activeProject );

};

#endif // SYNCHRONIZATIONCONTROLLER_H
