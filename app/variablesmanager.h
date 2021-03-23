/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VARIABLESMANAGER_H
#define VARIABLESMANAGER_H

#include <QObject>
#include "merginapi.h"
#include "qgsproject.h"
#include "qvariant.h"

class MerginApi;

/*
 * The class sets global and project variables related to Mergin.
 */
class VariablesManager : public QObject
{
  public:
    VariablesManager( MerginApi *merginApi, QObject *parent = nullptr );

    void removeMerginProjectVariables( QgsProject *project );

  public slots:
    void merginProjectChanged( QgsProject *project );

  private slots:
    void apiRootChanged();
    void authChanged();
    void setVersionVariable( const QString &projectFullName );

  private:
    MerginApi *mMerginApi = nullptr;
    QgsProject *mCurrentProject = nullptr;

    void setProjectVariables();
};

#endif // VARIABLESMANAGER_H
