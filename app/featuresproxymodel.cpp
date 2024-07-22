/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "featuresproxymodel.h"

FeaturesProxyModel::FeaturesProxyModel( QObject *parent ) : QSortFilterProxyModel( parent )
{
}

void FeaturesProxyModel::initialize()
{
  setSourceModel( mModel );

  // don't sort if there is no sort expression for the layer
  if ( mModel->sortingEnabled() )
  {
    setSortRole( FeaturesModel::SortValue );
    setSortCaseSensitivity( Qt::CaseInsensitive );
    sort( 0, mModel->sortOrder() );
  }
}

FeaturesModel *FeaturesProxyModel::featuresSourceModel() const
{
  return mModel;
}

void FeaturesProxyModel::setFeaturesSourceModel( FeaturesModel *sourceModel )
{
  if ( mModel == sourceModel )
    return;

  mModel = sourceModel;
  QObject::connect( mModel, &FeaturesModel::fetchingResultsChanged, this, [ = ]( bool pending ) { if ( !pending ) initialize(); } );
}
