/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHANGELOGMODEL_H
#define CHANGELOGMODEL_H

#include "QNetworkAccessManager"
#include <QAbstractListModel>
#include <QDate>
#include <QRegularExpression>

struct Changelog
{
  QString title;
  QString description;
  QString link;
  QDateTime date;

  QString descriptionWithoutImages() { return description.replace( QRegularExpression( "<img .*?>" ), "" ); };
};

class ChangelogModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS( MyRoles )

  public:
    enum MyRoles
    {
      TitleRole = Qt::UserRole + 1, DescriptionRole, LinkRole, DateRole
    };

    ChangelogModel( QObject *parent = nullptr );

    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

  private slots:
    void onFinished( QNetworkReply *reply );

  signals:
    void finished( const QString &title, const QString &link );
    void loadingFailure();

  private:
    QList<Changelog> mLogs;
    QNetworkAccessManager *mNetworkManager;
};

#endif // CHANGELOGMODEL_H
