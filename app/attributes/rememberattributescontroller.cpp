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
#include "activeproject.h"
#include "qgsattributes.h"
#include <QSettings>


RememberAttributesController::RememberAttributesController( QObject *parent )
  : QObject( parent )
{
}

RememberAttributesController::~RememberAttributesController() = default;

bool RememberAttributesController::rememberValuesAllowed() const
{
  QSettings settings;
  settings.beginGroup( CoreUtils::QSETTINGS_APP_GROUP_NAME );
  const bool reuseLastEnteredValues = settings.value( "reuseLastEnteredValues", false ).toBool();
  settings.endGroup();
  return reuseLastEnteredValues;
}

void RememberAttributesController::setRememberValuesAllowed( bool allowed )
{
  if ( rememberValuesAllowed() != allowed )
  {
    QSettings settings;
    settings.beginGroup( CoreUtils::QSETTINGS_APP_GROUP_NAME );
    settings.setValue( "reuseLastEnteredValues", allowed );
    settings.endGroup();
    emit rememberValuesAllowedChanged();
  }
}

void RememberAttributesController::storeFeature( const FeatureLayerPair &pair )
{
  const QgsVectorLayer *layer = pair.layer();
  const QgsFeature &feature = pair.feature();

  if ( !layer )
    return;

  QSettings settings;

  const QgsFields fields = layer->fields();
  for ( int fieldIndex = 0; fieldIndex < fields.count(); fieldIndex++ )
  {
    const QString fieldValueKey = keyForField( layer, fieldIndex );
    bool enabled = false;
    if ( rememberValuesAllowed() )
    {
      if ( settings.contains( fieldValueKey ) )
        enabled = true;
    }
    else
    {
      if ( layer->editFormConfig().reuseLastValue( fieldIndex ) )
        enabled = true;
    }

    if ( enabled )
    {
      const QVariant value = feature.attribute( fieldIndex );
      settings.setValue( fieldValueKey, value );
    }
    else
    {
      settings.remove( fieldValueKey );
    }
  }
}


bool RememberAttributesController::shouldRememberValue( const QgsVectorLayer *layer, int fieldIndex ) const
{
  if ( !layer )
    return false;

  QSettings settings;
  const QString fieldValueKey = keyForField( layer, fieldIndex );

  if ( rememberValuesAllowed() )
  {
    return settings.contains( fieldValueKey );
  }

  return false;
}

bool RememberAttributesController::setShouldRememberValue( const QgsVectorLayer *layer, int fieldIndex, bool shouldRemember )
{
  if ( !layer || !mActiveProject )
    return false;

  bool changed = false;
  QSettings settings;
  const QString fieldValueKey = keyForField( layer, fieldIndex );

  // rememberValuesAllowed() is always true, this method is called when toggling the checkboxes
  if ( settings.contains( fieldValueKey ) )
  {
    if ( !shouldRemember )
    {
      settings.remove( fieldValueKey );
      changed = true;
    }
  }
  else
  {
    if ( shouldRemember )
    {
      settings.setValue( fieldValueKey, QString() );
      changed = true;
    }
  }

  return changed;
}

bool RememberAttributesController::rememberedValue(
  const QgsVectorLayer *layer,
  int fieldIndex,
  QVariant &value ) const
{
  if ( !layer || !mActiveProject )
    return false;

  bool valueRead = false;
  QSettings settings;
  const QString fieldValueKey = keyForField( layer, fieldIndex );

  if ( settings.contains( fieldValueKey ) )
  {
    value = settings.value( fieldValueKey );
    valueRead = true;
  }

  return valueRead;
}

ActiveProject *RememberAttributesController::activeProject() const
{
  return mActiveProject;
}

void RememberAttributesController::setActiveProject( ActiveProject *newActiveProject )
{
  if ( mActiveProject == newActiveProject )
    return;
  mActiveProject = newActiveProject;
  emit activeProjectChanged( );
}

QString RememberAttributesController::keyForField( const QgsVectorLayer *layer, int fieldIndex ) const
{
  return QStringLiteral( "/%1/%2/%3/%4" ).arg( CoreUtils::CACHED_ATTRIBUTES_GROUP, mActiveProject->projectFullName(), layer->id(), QString::number( fieldIndex ) );
}
