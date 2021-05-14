/***************************************************************************
  rememberattributescontroller.cpp
  ----------------------------------------
  Date                 : 4.5.2021
  Copyright            : (C) 2021 by Peter Petrik
  Email                : zilolv@gmail.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "rememberattributescontroller.h"
#include "featurelayerpair.h"
#include "qgsattributes.h"
#include "featurelayerpair.h"

RememberAttributesController::RememberAttributesController( QObject *parent )
  : QObject( parent )
{
}

void RememberAttributesController::reset()
{
  mRememberedValues.clear();
}

RememberAttributesController::~RememberAttributesController() = default;

bool RememberAttributesController::rememberValuesAllowed() const
{
  return mRememberValuesAllowed;
}

void RememberAttributesController::setRememberValuesAllowed( bool rememberValuesAllowed )
{
  if ( mRememberValuesAllowed != rememberValuesAllowed )
  {
    mRememberValuesAllowed = rememberValuesAllowed;
    emit rememberValuesAllowedChanged();
  }
}

void RememberAttributesController::storeLayerFields( const QgsVectorLayer *layer )
{
  if ( layer && ( !mRememberedValues.contains( layer->id() ) ) )
  {
    mRememberedValues[layer->id()] = RememberedValues();
    mRememberedValues[layer->id()].attributeFilter.fill( false, layer->fields().size() );
  }
}

void RememberAttributesController::storeFeature( const FeatureLayerPair &pair )
{
  if ( pair.layer() )
    storeLayerFields( pair.layer() );
  mRememberedValues[pair.layer()->id()].feature = pair.feature();
}

bool RememberAttributesController::shouldRememberValue( const QgsVectorLayer *layer, int fieldIndex ) const
{
  // global switch off of the functionality
  if ( !mRememberValuesAllowed )
    return false;

  if ( !layer || !mRememberedValues.contains( layer->id() ) )
    return false;

  const RememberedValues from = mRememberedValues[layer->id()];
  if ( fieldIndex < 0 || fieldIndex >= from.attributeFilter.size() )
    // serious screw-up, mismatch between layer and stored layer?
    return false;

  return from.attributeFilter.at( fieldIndex );
}

bool RememberAttributesController::setShouldRememberValue( const QgsVectorLayer *layer, int fieldIndex, bool shouldRemember )
{
  // global switch off of the functionality
  if ( !mRememberValuesAllowed )
    return false;

  if ( layer && mRememberedValues.contains( layer->id() ) )
  {
    RememberedValues &from = mRememberedValues[layer->id()];
    if ( fieldIndex >= 0 && fieldIndex < from.attributeFilter.length() )
    {
      bool oldVal = from.attributeFilter[fieldIndex];
      if ( oldVal != shouldRemember )
      {
        from.attributeFilter[fieldIndex] = shouldRemember;
        return true;
      }
    }
  }
  return false;
}

bool RememberAttributesController::rememberedValue(
  const QgsVectorLayer *layer,
  int fieldIndex,
  QVariant &value ) const
{
  // global switch off of the functionality
  if ( !mRememberValuesAllowed )
    return false;

  if ( !layer || !mRememberedValues.contains( layer->id() ) )
    return false;

  const RememberedValues from = mRememberedValues[layer->id()];
  if ( !from.feature.isValid() )
    return false;

  QgsAttributes fromAttributes = from.feature.attributes();
  if ( fieldIndex < 0 || fieldIndex >= fromAttributes.length() )
    // serious screw-up, mismatch between layer and stored layer?
    return false;

  if ( !from.attributeFilter.at( fieldIndex ) )
    // user do not want to remember this value
    return false;

  // got it
  value = fromAttributes.at( fieldIndex );
  return true;
}


