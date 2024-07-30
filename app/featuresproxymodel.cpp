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
  setSortRole( FeaturesModel::SortValue );
  setSortCaseSensitivity( Qt::CaseInsensitive );
}

void FeaturesProxyModel::updateSorting()
{
  // don't sort if there is no sort expression for the layer
  if ( mModel->sortingEnabled() )
  {
    sort( 0, mModel->sortOrder() );
  }
  else
  {
    invalidate();
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

  if ( mModel )
    disconnect( mModel, nullptr, this, nullptr );

  mModel = sourceModel;
  setSourceModel( mModel );
  mModel->setupSorting();
  connect( mModel, &FeaturesModel::fetchingResultsChanged, this, [ = ]( bool pending ) { if ( !pending ) updateSorting(); } );
}
