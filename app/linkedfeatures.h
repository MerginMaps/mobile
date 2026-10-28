/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LINKEDFEATURES_H
#define LINKEDFEATURES_H

#include <QHash>

#include "qgsfeatureid.h"

class QgsVectorLayer;

/**
 * Finds and deletes child features linked to parent features by the relations of the layer's project
 */
class LinkedFeatures
{
  public:
    //! Returns TRUE if the feature has at least one child linked to it by a composition relation
    static bool hasCompositionChildren( QgsVectorLayer *layer, QgsFeatureId fid );

    /**
     * Deletes the children of the parent features \a fids from \a layer, including their nested children, and commits the child layers.
     * Children are followed through composition relations, or through all relations when \a allRelations is TRUE.
     * The parent features are not deleted and \a layer is not committed, the caller does that afterwards.
     * Returns FALSE and rolls back the child layers if a child could not be deleted.
     */
    static bool deleteChildFeatures( QgsVectorLayer *layer, const QgsFeatureIds &fids, bool allRelations = false );

  private:
    //! Adds the features and, recursively, their children to \a features
    static void collectFeatures( QgsVectorLayer *layer, const QgsFeatureIds &fids, bool allRelations, QHash<QgsVectorLayer *, QgsFeatureIds> &features );
};

#endif // LINKEDFEATURES_H
