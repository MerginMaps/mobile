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

QHash<QString, RememberAttributesController::RememberedValues> RememberAttributesController::sRememberedValues;

RememberAttributesController::RememberAttributesController( QObject *parent )
  : QObject( parent )
{
}

void RememberAttributesController::reset()
{
  sRememberedValues.clear();
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
  if ( layer && ( !sRememberedValues.contains( keyForLayer( layer ) ) ) )
  {
    sRememberedValues[keyForLayer( layer )] = RememberedValues();
    sRememberedValues[keyForLayer( layer )].attributeFilter.fill( false, layer->fields().size() );
  }
}

void RememberAttributesController::storeFeature( const FeatureLayerPair &pair )
{
  if ( pair.layer() )
    storeLayerFields( pair.layer() );
  sRememberedValues[keyForLayer( pair.layer() )].feature = pair.feature();
}

bool RememberAttributesController::shouldRememberValue( const QgsVectorLayer *layer, int fieldIndex ) const
{
  // global switch off of the functionality
  if ( !mRememberValuesAllowed )
    return false;

  if ( !layer || !sRememberedValues.contains( keyForLayer( layer ) ) )
    return false;

  const RememberedValues from = sRememberedValues[keyForLayer( layer )];
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

  if ( layer && sRememberedValues.contains( keyForLayer( layer ) ) )
  {
    RememberedValues &from = sRememberedValues[keyForLayer( layer )];
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

QString RememberAttributesController::keyForLayer( const QgsVectorLayer *layer ) const
{
  return QStringLiteral( "%1/%2" ).arg( mActiveProjectId, layer->id() );
}

bool RememberAttributesController::rememberedValue(
  const QgsVectorLayer *layer,
  int fieldIndex,
  QVariant &value ) const
{
  // global switch off of the functionality
  if ( !mRememberValuesAllowed )
    return false;

  if ( !layer || !sRememberedValues.contains( keyForLayer( layer ) ) )
    return false;

  const RememberedValues from = sRememberedValues[keyForLayer( layer )];
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
