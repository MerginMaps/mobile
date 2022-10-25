/***************************************************************************
  mapthemesmodel.h
  --------------------------------------
  Date                 : Oct 2018
  Copyright            : (C) 2018 by Viktor Sklencar
  Email                : vsklencar at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef MapThemesModel_H
#define MapThemesModel_H

#include <QStandardItemModel>

#include "qgsproject.h"

class MapThemesModel : public QStandardItemModel
{
    Q_OBJECT

    Q_PROPERTY( QgsProject *qgsProject READ qgsProject WRITE setQgsProject NOTIFY qgsProjectChanged )

  public:

    explicit MapThemesModel( QObject *parent = nullptr );
    virtual ~MapThemesModel();

    QgsProject *qgsProject() const;
    void setQgsProject( QgsProject *newQgsProject );

    Q_INVOKABLE void reset();

  signals:

    void qgsProjectChanged( QgsProject *qgsProject );

  private slots:
    void setupModel();

  private:
    QgsProject *mQgsProject = nullptr; // not owned
};

#endif // MapThemesModel_H
