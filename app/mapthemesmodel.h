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
    Q_PROPERTY( QList<QString> mapThemes READ mapThemes WRITE setMapThemes NOTIFY mapThemesReloaded )
    Q_PROPERTY( int activeThemeIndex READ activeThemeIndex WRITE setActiveThemeIndex NOTIFY activeThemeIndexChanged )

  public:
    enum Roles
    {
      Name = Qt::UserRole + 1
    };
    Q_ENUMS( Roles )

    explicit MapThemesModel( QObject *parent = nullptr );
    ~MapThemesModel();

    Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;
    Q_INVOKABLE QModelIndex index( int row, int column = 0, const QModelIndex &parent = QModelIndex() ) const override;
    Q_INVOKABLE void applyTheme( const QString &name );
    //! Returns row number of the first matching map theme name with the given name, otherwise return defaultRow.
    Q_INVOKABLE int rowAccordingName( QString name, int defaultRow = -1 ) const;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    QHash<int, QByteArray> roleNames() const override;

    QList<QString> mapThemes() const;
    void setMapThemes( const QList<QString> &mapThemes );

    int activeThemeIndex() const;
    /**
     * Sets and applies mapTheme if index is in mapThemes list length range.
     * \param activeThemeIndex index of the theme from the list of themes
     * \return Name of the newly activated map theme.
     */
    QString setActiveThemeIndex( int activeThemeIndex );
    /**
     * Sets active map theme according given name
     * \param name QString represents map theme name
     */
    void updateMapTheme( const QString name );
    void reloadMapThemes( QgsProject *project );

  signals:
    void mapThemesReloaded();
    void mapThemeChanged( const QString &name );
    void activeThemeIndexChanged();

  private:
    QgsProject *mProject = nullptr;
    QList<QString> mMapThemes;
    int mActiveThemeIndex = -1;

    /**
    * Updates map theme (sets active index in the model) with the first match from project's theme collection.
    * Precondition is to have model up-to-date with Project's theme collection.
    */
    void updateMapThemeByProject();
};

#endif // MapThemesModel_H
