/***************************************************************************
  qgsquicklayertreemodel.h
  --------------------------------------
  Date                 : Nov 2017
  Copyright            : (C) 2017 by Peter Petrik
  Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef LAYERSMODEL_H
#define LAYERSMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QSet>

class QgsMapLayer;
class QgsProject;

class LayersModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( QList<QgsMapLayer *> layers READ layers NOTIFY layersReloaded )
    Q_PROPERTY( int activeIndex READ activeIndex WRITE setActiveIndex NOTIFY activeIndexChanged )

  public:
    enum Roles
    {
      Name = Qt::UserRole + 1,
      isVector,
      isReadOnly,
      IconSource,
      VectorLayer,
      HasGeometry
    };
    Q_ENUMS( Roles )

    explicit LayersModel( QObject *parent = nullptr );
    ~LayersModel() override;

    Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;
    Q_INVOKABLE QModelIndex index( int row, int column = 0, const QModelIndex &parent = QModelIndex() ) const override;
    Q_INVOKABLE int rowAccordingName( QString name, int defaultRow = -1 ) const;
    Q_INVOKABLE int noOfEditableLayers() const;
    Q_INVOKABLE int firstWritableLayerIndex() const;
    //! Returns nullptr pointer to activeLayer according mActiveIndex
    Q_INVOKABLE QgsMapLayer *activeLayer();

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    QHash<int, QByteArray> roleNames() const override;

    QList<QgsMapLayer *> layers() const;

    int activeIndex() const;
    /**
     * Sets layer if index is in layers list length range.
     * \param activeIndex index of the layder from mLayers
     * \return Name of the activated layer.
     */
    QString setActiveIndex( int activeIndex );
    /**
     * Sets active layer according given name
     * \param name QString represents layer name
     */
    void updateActiveLayer( const QString &name );
    void reloadLayers( QgsProject *project ); //when project file changes, reload all layers, etc.

  signals:
    void layersReloaded();
    void activeIndexChanged();

  private:
    QList<QgsMapLayer *> mLayers; // all layers
    int mActiveIndex = -1;
};

#endif // LAYERSMODEL_H
