/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SPLITTINGMAPTOOL_H
#define SPLITTINGMAPTOOL_H

#include "abstractmaptool.h"
#include <qglobal.h>

#include "qgsgeometry.h"

#include "featurelayerpair.h"

class SplittingMapTool : public AbstractMapTool
{
    Q_OBJECT

    Q_PROPERTY( QgsGeometry recordedGeometry READ recordedGeometry WRITE setRecordedGeometry NOTIFY recordedGeometryChanged )
    Q_PROPERTY( FeatureLayerPair featureToSplit READ featureToSplit WRITE setFeatureToSplit NOTIFY featureToSplitChanged )

  public:

    enum SplitResult
    {
      Success = 0,
      InvalidSplit = 1, // Split line does not properly cross the feature boundary
      Failed = 2
    };
    Q_ENUM( SplitResult )

    explicit SplittingMapTool( QObject *parent = nullptr );
    virtual ~SplittingMapTool() override;

    /**
     * Adds point to the end of the recorded geometry; updates recordedGeometry afterwards
     * Passed point needs to be in active vector layer CRS
     */
    Q_INVOKABLE void addPoint( const QgsPoint &point );

    /**
     *  Removes last point from recorded geometry if there is at least one point
     *  Updates recordedGeometry afterwards
     */
    Q_INVOKABLE void removePoint();

    //! Returns true if the captured geometry has enought points for the specified layer
    Q_INVOKABLE bool hasValidGeometry() const;

    //! Splits the feature with recorded geometry and commits the changes to the layer
    Q_INVOKABLE SplittingMapTool::SplitResult commitSplit() const;

    // Getters/setters
    const QgsGeometry &recordedGeometry() const;
    void setRecordedGeometry( const QgsGeometry &newRecordedGeometry );

    const FeatureLayerPair &featureToSplit() const;
    void setFeatureToSplit( const FeatureLayerPair &newFeatureToSplit );

  signals:
    void recordedGeometryChanged( const QgsGeometry &recordedGeometry );
    void featureToSplitChanged( const FeatureLayerPair &featureToSplit );

  protected:
    void rebuildGeometry();

  private:
    /**
     * Validates if the recorded geometry can successfully split the feature
     * Returns true if the split line properly crosses the feature boundary
     */
    bool isValidSplit() const;

    QVector<QgsPoint> mPoints;

    QgsGeometry mRecordedGeometry;
    FeatureLayerPair mFeatureToSplit;
};

#endif // SPLITTINGMAPTOOL_H
