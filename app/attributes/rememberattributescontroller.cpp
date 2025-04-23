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
#include "coreutils.h"
#include "qgsattributes.h"
#include <QSettings>


RememberAttributesController::RememberAttributesController( QObject *parent )
  : QObject( parent )
{
}

RememberAttributesController::~RememberAttributesController() = default;

void RememberAttributesController::storeFeature( const FeatureLayerPair &pair )
{
  const QgsVectorLayer *layer = pair.layer();
  const QgsFeature &feature = pair.feature();

  if ( !layer || mActiveProjectId.isEmpty() )
    return;

  const QgsFields fields = layer->fields();
  for ( int fieldIndex = 0; fieldIndex < fields.count(); fieldIndex++ )
  {
    const QString fieldValueKey = keyForField( layer, fieldIndex );
    if ( mRememberValuesAllowed )
    {
      // if key does not exist, default to invalid QVariant, meaning "unchecked"
      // if key value is valid, replace it with the new value from the feature
      if ( mSettings.value( fieldValueKey, QVariant() ).isValid() )
        mSettings.setValue( fieldValueKey, feature.attribute( fieldIndex ) );
      // else do nothing, keep invalid value in settings, meaning "unchecked"

      continue;
    }

    // we still need to store the value when qgis says so, even if mRememberValuesAllowed is false
    if ( enabledInQgis( layer, fieldIndex ) )
    {
      mSettings.setValue( fieldValueKey, feature.attribute( fieldIndex ) );
    }
    else
    {
      // Let's tidy up and not keep unneeded settings entries
      mSettings.remove( fieldValueKey );
    }
  }
}


bool RememberAttributesController::shouldRememberValue( const QgsVectorLayer *layer, int fieldIndex ) const
{
  if ( !mRememberValuesAllowed || !layer )
    return false;

  const QString fieldValueKey = keyForField( layer, fieldIndex );

  if ( mSettings.contains( fieldValueKey ) )
  {
    if ( mSettings.value( fieldValueKey ).isValid() )
      return true;
    else
      return false;
  }
  else if ( enabledInQgis( layer, fieldIndex ) )
  {
    return true;
  }

  return false;
}

bool RememberAttributesController::setShouldRememberValue( const QgsVectorLayer *layer, int fieldIndex, bool shouldRemember )
{
  if ( !layer || mActiveProjectId.isEmpty() )
    return false;

  const QString fieldValueKey = keyForField( layer, fieldIndex );

  const bool wasChecked = mSettings.value( fieldValueKey, QVariant() ).isValid();
  // mRrememberValuesAllowed is always true, this method is called when toggling the checkboxes

  if ( shouldRemember )
  {
    if ( !mSettings.value( fieldValueKey, QVariant() ).isValid() )
    {
      // we either have no value stored, or an invalid one, meaning "unchecked".
      // Store a valid empty string, meaning "checked but no stored value yet"
      mSettings.setValue( fieldValueKey, QString() );
    }
  }
  else
  {
    // we should not remember, set to invalid value, meaning "unchecked"
    mSettings.setValue( fieldValueKey, QVariant() );
  }

  return wasChecked != shouldRemember;
}

bool RememberAttributesController::rememberedValue(
  const QgsVectorLayer *layer,
  int fieldIndex,
  QVariant &value ) const
{
  if ( !layer || mActiveProjectId.isEmpty() )
    return false;

  const QString fieldValueKey = keyForField( layer, fieldIndex );

  if ( !mRememberValuesAllowed )
  {
    if ( !enabledInQgis( layer, fieldIndex ) )
      return false;
  }

  if ( !mSettings.value( fieldValueKey, QVariant() ).isValid() )
  {
    return false;
  }

  value = mSettings.value( fieldValueKey );

  return true;
}

QString RememberAttributesController::keyForField( const QgsVectorLayer *layer, int fieldIndex ) const
{
  return QStringLiteral( "/%1/%2/%3/%4" ).arg( CoreUtils::CACHED_ATTRIBUTES_GROUP, mActiveProjectId, layer->id(), QString::number( fieldIndex ) );
}

bool RememberAttributesController::enabledInQgis( const QgsVectorLayer *layer, int fieldIndex ) const
{
  return layer->editFormConfig().reuseLastValue( fieldIndex );
}
