/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "splittingmaptool.h"

#include "qgsvectorlayer.h"

SplittingMapTool::SplittingMapTool( QObject *parent )
  : AbstractMapTool{parent}
{

}

SplittingMapTool::~SplittingMapTool() = default;

void SplittingMapTool::addPoint( const QgsPoint &point )
{
  if ( !mapSettings() || !mFeatureToSplit.isValid() )
    return;

  mPoints.push_back( point );
  rebuildGeometry();
}

void SplittingMapTool::removePoint()
{
  if ( !mPoints.isEmpty() )
  {
    mPoints.pop_back();
    rebuildGeometry();
  }
}

bool SplittingMapTool::hasValidGeometry() const
{
  return mPoints.count() >= 2;
}

bool SplittingMapTool::isValidSplit() const
{
  if ( !mFeatureToSplit.isValid() || !hasValidGeometry() )
    return false;

  const QgsGeometry featureGeom = mFeatureToSplit.feature().geometry();
  if ( featureGeom.isNull() || featureGeom.isEmpty() )
    return false;

  // split line must intersect the feature's geometry
  if ( !mRecordedGeometry.intersects( featureGeom ) )
    return false;

  QgsPointXY firstPoint( mPoints.first() );
  QgsPointXY lastPoint( mPoints.last() );

  // start and end points of the line must be outside the feature, ensuring line properly crosses the feature
  if ( featureGeom.contains( &firstPoint ) || featureGeom.contains( &lastPoint ) )
    return false;

  return true;
}

SplittingMapTool::SplitResult SplittingMapTool::commitSplit() const
{
  if ( !mFeatureToSplit.isValid() )
    return Failed;

  if ( !hasValidGeometry() )
    return Failed;

  if ( !isValidSplit() )
    return InvalidSplit;

  // only the specified featureToSplit shall be split, so we select
  // it here in order to avoid other features being split
  mFeatureToSplit.layer()->select( mFeatureToSplit.feature().id() );
  mFeatureToSplit.layer()->startEditing();

  Qgis::GeometryOperationResult result = mFeatureToSplit.layer()->splitFeatures( mPoints );

  mFeatureToSplit.layer()->commitChanges();
  mFeatureToSplit.layer()->deselect( mFeatureToSplit.feature().id() );

  if ( result == Qgis::GeometryOperationResult::Success )
  {
    return Success;
  }
  return Failed;
}

void SplittingMapTool::rebuildGeometry()
{
  QgsGeometry geometry;

  if ( mPoints.count() > 0 )
  {
    geometry = QgsGeometry::fromPolyline( mPoints );
  }

  setRecordedGeometry( geometry );
}

// Getters / setters
const QgsGeometry &SplittingMapTool::recordedGeometry() const
{
  return mRecordedGeometry;
}

void SplittingMapTool::setRecordedGeometry( const QgsGeometry &newRecordedGeometry )
{
  if ( mRecordedGeometry.equals( newRecordedGeometry ) )
    return;
  mRecordedGeometry = newRecordedGeometry;
  emit recordedGeometryChanged( mRecordedGeometry );
}

const FeatureLayerPair &SplittingMapTool::featureToSplit() const
{
  return mFeatureToSplit;
}

void SplittingMapTool::setFeatureToSplit( const FeatureLayerPair &newFeatureToSplit )
{
  if ( mFeatureToSplit == newFeatureToSplit )
    return;
  mFeatureToSplit = newFeatureToSplit;
  emit featureToSplitChanged( mFeatureToSplit );
}
