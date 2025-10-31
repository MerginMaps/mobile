/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AUTOSYNCCONTROLLER_H
#define AUTOSYNCCONTROLLER_H

#include <QObject>

class QgsProject;

class AutosyncController : public QObject
{
    Q_OBJECT

  public:

    explicit AutosyncController( QgsProject *openedQgsProject, QObject *parent = nullptr );
    virtual ~AutosyncController();

  signals:

    void projectChangeDetected();

  private:

    QgsProject *mQgsProject = nullptr; // not owned
};

#endif // AUTOSYNCCONTROLLER_H
