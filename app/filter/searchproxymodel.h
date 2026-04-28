/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SEARCHPROXYMODEL_H
#define SEARCHPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

class SearchProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY( QString searchString READ searchString WRITE setSearchString NOTIFY searchStringChanged )

  public:
    explicit SearchProxyModel( QObject *parent = nullptr );

    QString searchString() const;
    void setSearchString( const QString &newSearchString );

  signals:
    void searchStringChanged();

  private:
    QString mSearchString;
};

#endif // SEARCHPROXYMODEL_H
