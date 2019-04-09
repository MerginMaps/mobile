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

#include <QAbstractListModel>
#include <QList>
#include <QSet>

class QgsMapLayer;
class QgsProject;

class MapThemesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( QList<QString> mapThemes READ mapThemes WRITE setMapThemes NOTIFY mapThemesChanged )

  public:
    enum Roles
    {
      Name = Qt::UserRole + 1
    };
    Q_ENUMS( Roles )

    explicit MapThemesModel( QgsProject *project, QObject *parent = nullptr );
    ~MapThemesModel();

    Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;
    Q_INVOKABLE QModelIndex index( int row ) const;
    Q_INVOKABLE void applyTheme( const QString &name );

    int rowCount( const QModelIndex &parent = QModelIndex() ) const;

    QHash<int, QByteArray> roleNames() const override;

    QList<QString> mapThemes() const;
    void setMapThemes( const QList<QString> &mapThemes );

  signals:
    void mapThemesChanged();
    void reloadLayers();

  public slots:
    void reloadMapThemes();

  private:
    QgsProject *mProject;
    QList<QString> mMapThemes;
};

#endif // MapThemesModel_H
