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
#include <QList>

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

    //! Commits every layer in \a handledLayers other than \a layer itself, rolling all back if one is not editable or fails to commit
    static bool commitCascadedChildren( QgsVectorLayer *layer, const QList<QgsVectorLayer *> &handledLayers );

    //! Returns \a layer and the layers QGIS's own cascade delete actually left editable; use in place of DeleteContext::handledLayers(), which misses nested cascades
    static QList<QgsVectorLayer *> editedCompositionLayers( QgsVectorLayer *layer );

    //! Deletes the children of \a fids from \a layer through every relation; used only when discarding a new feature
    static bool deleteAllLinkedChildFeatures( QgsVectorLayer *layer, const QgsFeatureIds &fids );

  private:
    //! Adds the features and, recursively, their children to \a features
    static void collectFeatures( QgsVectorLayer *layer, const QgsFeatureIds &fids, QHash<QgsVectorLayer *, QgsFeatureIds> &features );

    //! Returns \a layer and every layer reachable from it through composition relations
    static QList<QgsVectorLayer *> compositionLayerTree( QgsVectorLayer *layer );
};

#endif // LINKEDFEATURES_H
