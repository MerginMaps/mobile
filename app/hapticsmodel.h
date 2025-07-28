/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HAPTICSMODEL_H
#define HAPTICSMODEL_H

#include <QAbstractListModel>

#include "appsettings.h"

class HapticsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    // we need count property as it gets accessed in QML and QAbstractListModel doesn't have it,
    // while QML ListModel has it by default
    Q_PROPERTY( int count READ count NOTIFY countChanged )


  public:
    explicit HapticsModel( QObject *parent = nullptr );
    ~HapticsModel() override = default;

    enum HapticsRoles
    {
      ValueRole = Qt::UserRole + 1,
      TextRole
    };
    Q_ENUM( HapticsRoles )

    int rowCount( const QModelIndex &parent ) const override;
    QVariant data( const QModelIndex &index, int role ) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;
  signals:
    void countChanged( int count );

  private:
    QList<std::pair<AppSettings::HapticsType, QString>> mHapticsTypes;
};



#endif //HAPTICSMODEL_H
