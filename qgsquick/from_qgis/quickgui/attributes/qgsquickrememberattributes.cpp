/***************************************************************************
  qgsquickrememberattributes.cpp
  --------------------------------------
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

#include "qgsquickrememberattributes.h"
#include "qgsquickfeaturelayerpair.h"
#include "qgsattributes.h"

QgsQuickRememberAttributes::QgsQuickRememberAttributes( QObject *parent )
  : QObject( parent )
{
}

QgsQuickRememberAttributes::~QgsQuickRememberAttributes() = default;

bool QgsQuickRememberAttributes::rememberValuesAllowed() const
{
  return mRememberValuesAllowed;
}

void QgsQuickRememberAttributes::setRememberValuesAllowed( bool rememberValuesAllowed )
{
  if ( mRememberValuesAllowed != rememberValuesAllowed )
  {
    mRememberValuesAllowed = rememberValuesAllowed;
    emit rememberValuesAllowedChanged();
  }
}

void QgsQuickRememberAttributes::storeLayerFields( const QgsVectorLayer *layer )
{
  if ( layer && ( !mRememberedValues.contains( layer->id() ) ) )
    mRememberedValues[layer->id()].attributeFilter.fill( false, layer->fields().size() );
}

bool QgsQuickRememberAttributes::shouldRememberValue( const QgsVectorLayer *layer, int fieldIndex ) const
{
  QVariant dummy;
  return rememberedValue(
           layer,
           fieldIndex,
           dummy );
}

bool QgsQuickRememberAttributes::setShouldRememberValue( const QgsVectorLayer *layer, int fieldIndex, bool shouldRemember )
{
  // global switch off of the functionality
  if ( mRememberValuesAllowed )
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

bool QgsQuickRememberAttributes::rememberedValue(
  const QgsVectorLayer *layer,
  int fieldIndex,
  QVariant &value ) const
{
  // global switch off of the functionality
  if ( mRememberValuesAllowed )
    return false;

  if ( !layer && !mRememberedValues.contains( layer->id() ) )
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


