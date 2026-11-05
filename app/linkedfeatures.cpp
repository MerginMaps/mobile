/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "linkedfeatures.h"
#include "coreutils.h"

#include "qgsproject.h"
#include "qgsrelationmanager.h"
#include "qgsvectorlayer.h"

bool LinkedFeatures::hasCompositionChildren( QgsVectorLayer *layer, QgsFeatureId fid )
{
  if ( !layer || !layer->project() )
    return false;

  const QgsFeature feature = layer->getFeature( fid );
  if ( !feature.isValid() )
    return false;

  const QList<QgsRelation> relations = layer->project()->relationManager()->referencedRelations( layer );
  for ( const QgsRelation &relation : relations )
  {
    QgsVectorLayer *childLayer = relation.referencingLayer();
    if ( relation.strength() != Qgis::RelationshipStrength::Composition || !childLayer )
      continue;

    QgsFeatureRequest request = relation.getRelatedFeaturesRequest( feature );
    request.setFlags( Qgis::FeatureRequestFlag::NoGeometry ).setNoAttributes().setLimit( 1 );

    QgsFeature child;
    if ( childLayer->getFeatures( request ).nextFeature( child ) )
      return true;
  }

  return false;
}

bool LinkedFeatures::commitCascadedChildren( QgsVectorLayer *layer, const QList<QgsVectorLayer *> &handledLayers )
{
  QList<QgsVectorLayer *> childLayers;
  for ( QgsVectorLayer *handledLayer : handledLayers )
  {
    if ( handledLayer != layer )
      childLayers << handledLayer;
  }

  // QGIS silently ignores a failed startEditing() on a child layer, so its features were not deleted
  for ( QgsVectorLayer *childLayer : std::as_const( childLayers ) )
  {
    if ( !childLayer->isEditable() )
    {
      CoreUtils::log( QStringLiteral( "Delete linked features" ), QStringLiteral( "Could not start editing layer %1" ).arg( childLayer->name() ) );
      return false;
    }
  }

  for ( QgsVectorLayer *childLayer : std::as_const( childLayers ) )
  {
    if ( !childLayer->commitChanges() )
    {
      CoreUtils::log( QStringLiteral( "Delete linked features" ), QStringLiteral( "Failed to commit changes of layer %1:\n%2" )
                      .arg( childLayer->name(), childLayer->commitErrors().join( QLatin1Char( '\n' ) ) ) );

      for ( QgsVectorLayer *editedLayer : std::as_const( childLayers ) )
      {
        if ( editedLayer->isEditable() )
          editedLayer->rollBack();
      }
      return false;
    }

    childLayer->triggerRepaint();
  }

  return true;
}

QList<QgsVectorLayer *> LinkedFeatures::editedCompositionLayers( QgsVectorLayer *layer )
{
  QList<QgsVectorLayer *> editedLayers;
  const QList<QgsVectorLayer *> compositionLayers = compositionLayerTree( layer );
  for ( QgsVectorLayer *compositionLayer : compositionLayers )
  {
    if ( compositionLayer->isEditable() )
      editedLayers << compositionLayer;
  }
  return editedLayers;
}

QList<QgsVectorLayer *> LinkedFeatures::compositionLayerTree( QgsVectorLayer *layer )
{
  QList<QgsVectorLayer *> layers = { layer };
  for ( int i = 0; i < layers.size(); ++i )
  {
    QgsVectorLayer *currentLayer = layers.at( i );
    if ( !currentLayer->project() )
      continue;

    const QList<QgsRelation> relations = currentLayer->project()->relationManager()->referencedRelations( currentLayer );
    for ( const QgsRelation &relation : relations )
    {
      QgsVectorLayer *childLayer = relation.referencingLayer();
      if ( relation.strength() == Qgis::RelationshipStrength::Composition && childLayer && !layers.contains( childLayer ) )
        layers << childLayer;
    }
  }

  return layers;
}

bool LinkedFeatures::deleteAllLinkedChildFeatures( QgsVectorLayer *layer, const QgsFeatureIds &fids )
{
  if ( !layer || fids.isEmpty() )
    return false;

  QHash<QgsVectorLayer *, QgsFeatureIds> features;
  collectFeatures( layer, fids, features );

  // the parent features are deleted by the caller
  features[layer].subtract( fids );

  // delete from all layers before committing any of them, so a failed deletion changes nothing
  QList<QgsVectorLayer *> editedLayers;
  bool deleted = true;
  const QList<QgsVectorLayer *> layers = features.keys();
  for ( QgsVectorLayer *layerToEdit : layers )
  {
    const QgsFeatureIds fidsToDelete = features.value( layerToEdit );
    if ( fidsToDelete.isEmpty() )
      continue;

    if ( !layerToEdit->isEditable() && !layerToEdit->startEditing() )
    {
      CoreUtils::log( QStringLiteral( "Delete linked features" ), QStringLiteral( "Could not start editing layer %1" ).arg( layerToEdit->name() ) );
      deleted = false;
      break;
    }

    editedLayers << layerToEdit;

    if ( !layerToEdit->deleteFeatures( fidsToDelete ) )
    {
      CoreUtils::log( QStringLiteral( "Delete linked features" ), QStringLiteral( "Could not delete features from layer %1" ).arg( layerToEdit->name() ) );
      deleted = false;
      break;
    }
  }

  if ( !deleted )
  {
    for ( QgsVectorLayer *editedLayer : std::as_const( editedLayers ) )
    {
      editedLayer->rollBack();
    }
    return false;
  }

  // layer is committed by the caller after the parent features are deleted
  editedLayers.removeAll( layer );
  return commitCascadedChildren( layer, editedLayers );
}

void LinkedFeatures::collectFeatures( QgsVectorLayer *layer, const QgsFeatureIds &fids, QHash<QgsVectorLayer *, QgsFeatureIds> &features )
{
  // skipping already collected features also stops cyclic relations
  const QgsFeatureIds newFids = fids - features.value( layer );
  if ( newFids.isEmpty() || !layer->project() )
    return;

  features[layer].unite( newFids );

  const QList<QgsRelation> relations = layer->project()->relationManager()->referencedRelations( layer );
  for ( const QgsRelation &relation : relations )
  {
    QgsVectorLayer *childLayer = relation.referencingLayer();
    if ( !childLayer )
      continue;

    QgsFeatureIds childFids;
    QgsFeatureIterator parentIt = layer->getFeatures( QgsFeatureRequest().setFilterFids( newFids ) );
    QgsFeature parent;
    while ( parentIt.nextFeature( parent ) )
    {
      QgsFeatureRequest childRequest = relation.getRelatedFeaturesRequest( parent );
      childRequest.setFlags( Qgis::FeatureRequestFlag::NoGeometry ).setNoAttributes();

      QgsFeatureIterator childIt = childLayer->getFeatures( childRequest );
      QgsFeature child;
      while ( childIt.nextFeature( child ) )
      {
        childFids.insert( child.id() );
      }
    }

    collectFeatures( childLayer, childFids, features );
  }
}
