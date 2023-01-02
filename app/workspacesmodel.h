/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WORKSPACESMODEL_H
#define WORKSPACESMODEL_H

#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include "merginapi.h"

class WorkspacesModel : public QStandardItemModel
{
    Q_OBJECT

    Q_PROPERTY( MerginApi *merginApi READ merginApi WRITE setMerginApi NOTIFY merginApiChanged )

  public:

    explicit WorkspacesModel( QObject *parent = nullptr );
    virtual ~WorkspacesModel();

    void rebuild();
    MerginApi *merginApi() const;
    void setMerginApi( MerginApi *merginApi );

  signals:
    void merginApiChanged( MerginApi *api );

  private:
    MerginApi *mApi = nullptr; // not owned
};

#endif // WORKSPACESMODEL_H
