/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FEATURESPROXYMODEL_H
#define FEATURESPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

#include "inputconfig.h"
#include "featuresmodel.h"

/**
 * \brief The FeaturesProxyModel class used as a proxy sort model for the \see FeaturesModel class.
 *
 * FeaturesProxyModel is a QML type with required property of FeatureSourceModel. Without source model, this model does nothing (is not initialized).
 * After setting source model, this model starts sorting if sortingEnabled() returns true for the source model.
 */
class FeaturesProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY( FeaturesModel *featuresSourceModel READ featuresSourceModel WRITE setFeaturesSourceModel )

  public:
    explicit FeaturesProxyModel( QObject *parent = nullptr );
    ~FeaturesProxyModel() override {};

    FeaturesModel *featuresSourceModel() const;

  public slots:
    void setFeaturesSourceModel( FeaturesModel *sourceModel );

  private:
    void initialize();

    FeaturesModel *mModel = nullptr; // not owned by this, needs to be set in order to proxy model to work

    Qt::SortOrder mSortOrder = Qt::SortOrder::AscendingOrder;
    QString mSortExpression;

    friend class TestModels;
};

#endif // FEATURESPROXYMODEL_H
