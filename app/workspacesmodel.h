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
    Q_PROPERTY( bool isLoading READ isLoading NOTIFY isLoadingChanged )

  public:

    explicit WorkspacesModel( QObject *parent = nullptr );
    virtual ~WorkspacesModel();

    MerginApi *merginApi() const;
    void setMerginApi( MerginApi *merginApi );

    bool isLoading() const;

  public slots:
    void listWorkspaces();
    void onListWorkspacesFinished( const QMap<int, QString> &workspaces );

  signals:
    void merginApiChanged( MerginApi *api );
    void isLoadingChanged( bool isLoading );
    void modelInitialized();

  private:
    void setModelIsLoading( bool state );
    void initializeModel();

    bool mModelIsLoading;
    MerginApi *mApi = nullptr; // not owned
};

#endif // WORKSPACESMODEL_H
