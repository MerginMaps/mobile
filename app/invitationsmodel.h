/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INVITATIONSMODEL_H
#define INVITATIONSMODEL_H

#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include "merginapi.h"
#include "merginuserinfo.h"

class InvitationsModel : public QStandardItemModel
{
    Q_OBJECT

    Q_PROPERTY( MerginApi *merginApi READ merginApi WRITE setMerginApi NOTIFY merginApiChanged )
    Q_PROPERTY( bool isLoading READ isLoading NOTIFY isLoadingChanged )

  public:

    explicit InvitationsModel( QObject *parent = nullptr );
    virtual ~InvitationsModel();

    Q_INVOKABLE void listInvitations();

    MerginApi *merginApi() const;
    void setMerginApi( MerginApi *merginApi );

    bool isLoading() const;

  public slots:
    void onListInvitationsFinished( const QList<MerginInvitation> &invitations );

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

#endif // INVITATIONSMODEL_H
