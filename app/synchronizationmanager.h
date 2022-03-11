/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SYNCHRONIZATIONMANAGER_H
#define SYNCHRONIZATIONMANAGER_H

#include <QObject>
#include <qglobal.h>

#include <project.h>

class SynchronizationManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool autosyncAllowed READ autosyncAllowed WRITE setAutosyncAllowed NOTIFY autosyncAllowedChanged )

  public:
    explicit SynchronizationManager( QObject *parent = nullptr );

    bool autosyncAllowed();
    void setAutosyncAllowed( bool );

  signals:
    void autosyncAllowedChanged( bool autosyncAllowed );

  public slots:
    void activeProjectChanged( LocalProject activeProject );

  private:
    bool mAutosyncAllowed = false;

};

#endif // SYNCHRONIZATIONMANAGER_H
