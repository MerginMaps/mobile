/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INVITATIONSPROXYMODEL_H
#define INVITATIONSPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

#include "invitationsmodel.h"

class InvitationsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY( InvitationsModel *invitationsSourceModel READ invitationsSourceModel WRITE setInvitationsSourceModel NOTIFY invitationsSourceModelChanged )

  public:
    explicit InvitationsProxyModel( QObject *parent = nullptr );
    ~InvitationsProxyModel() override {};

    InvitationsModel *invitationsSourceModel() const;

    bool filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const override;

  public slots:
    void setInvitationsSourceModel( InvitationsModel *sourceModel );

  signals:
    void invitationsSourceModelChanged();

  private:
    InvitationsModel *mModel = nullptr; // not owned by this, needs to be set in order to proxy model to work
};

#endif // INVITATIONSPROXYMODEL_H
