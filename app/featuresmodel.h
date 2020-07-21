/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef FEATURESMODEL_H
#define FEATURESMODEL_H

#include <QAbstractListModel>
#include "loader.h"

#include "qgsmaplayer.h"
#include "qgsvectorlayer.h"
#include "qgsfeaturerequest.h"
#include "qgsfeatureiterator.h"
#include "qgsquickfeaturelayerpair.h"
#include "qstringliteral.h"

/**
 * List model to populate feature list from selected layer
 */
class FeaturesModel : public QAbstractListModel
{
    Q_OBJECT

    enum roleNames
    {
      FeatureTitle = Qt::UserRole + 1,
      FeatureId,
      Description, // secondary text in list view
      GeometryType, // type of geometry (point, line, ..)
      IconSource
    };

  public:
    explicit FeaturesModel( Loader &loader, QObject *parent = nullptr );
    ~FeaturesModel() override {};

    //! Function to get QgsQuickFeatureLayerPair by feature id
    Q_INVOKABLE QgsQuickFeatureLayerPair featureLayerPair( const int &featureId );

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    QHash<int, QByteArray> roleNames() const override;


    bool setData( const QModelIndex &index, const QVariant &value,
                  int role = Qt::EditRole ) override;
    Qt::ItemFlags flags( const QModelIndex &index ) const override;

  public slots:
    void reloadDataFromLayer( QgsVectorLayer *layer );

    void activeProjectChanged();
    void activeMapThemeChanged( const QString &mapTheme );


  private:
    //! Empty data when changing map theme or project
    void emptyData();

    QList<QgsQuickFeatureLayerPair> mFeatures;
    Loader &mLoader;
};

#endif // FEATURESMODEL_H
