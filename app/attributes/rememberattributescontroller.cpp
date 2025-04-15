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
#include <coreutils.h>
#include <QSettings>


const QString RememberAttributesController::CACHED_ATTRIBUTES_GROUP = QStringLiteral( "cachedAttributesValue" );

RememberAttributesController::RememberAttributesController( QObject *parent )
  : QObject( parent )
{
}

RememberAttributesController::~RememberAttributesController() = default;

bool RememberAttributesController::rememberValuesAllowed() const
{
  QSettings settings;
  settings.beginGroup( CoreUtils::QSETTINGS_APP_GROUP_NAME );
  bool reuseLastEnteredValues = settings.value( "reuseLastEnteredValues", false ).toBool();
  settings.endGroup();
  return reuseLastEnteredValues;
}

void RememberAttributesController::setRememberValuesAllowed( bool rememberValuesAllowed )
{
  if ( this->rememberValuesAllowed() != rememberValuesAllowed )
  {
    QSettings settings;
    settings.beginGroup( CoreUtils::QSETTINGS_APP_GROUP_NAME );
    settings.setValue( "reuseLastEnteredValues", rememberValuesAllowed );
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
  settings.beginGroup( CACHED_ATTRIBUTES_GROUP );

  const QgsFields &fields = layer->fields();
  for ( int fieldIndex = 0; fieldIndex < fields.count(); fieldIndex++ )
  {
    QString fieldEnabledKey = QStringLiteral( "/%1/%2/%3/enabled" ).arg( mActiveProject->projectFullName() ).arg( layer->id() ).arg( fieldIndex );
    bool fieldEnabled = settings.value( fieldEnabledKey, false ).toBool();

    if ( fieldEnabled )
    {
      QString fieldValueKey = QStringLiteral( "/%1/%2/%3/value" ).arg( mActiveProject->projectFullName() ).arg( layer->id() ).arg( fieldIndex );
      QVariant value = feature.attribute( fieldIndex );
      settings.setValue( fieldValueKey, value );
    }
  }

  settings.endGroup();
}

bool RememberAttributesController::shouldRememberValue( const QgsVectorLayer *layer, int fieldIndex ) const
{
  // global switch off of the functionality
  if ( !rememberValuesAllowed() )
    return false;

  if ( !layer )
    return false;

  QSettings settings;
  settings.beginGroup( CACHED_ATTRIBUTES_GROUP );

  QString fieldEnabledKey = QStringLiteral( "/%1/%2/%3/enabled" ).arg( mActiveProject->projectFullName() ).arg( layer->id() ).arg( fieldIndex );
  bool fieldEnabled = settings.value( fieldEnabledKey, false ).toBool();;

  settings.endGroup();

  return fieldEnabled;
}

bool RememberAttributesController::setShouldRememberValue( const QgsVectorLayer *layer, int fieldIndex, bool shouldRemember )
{
  if ( !rememberValuesAllowed() )
    return false;

  QSettings settings;
  settings.beginGroup( CACHED_ATTRIBUTES_GROUP );

  QString fieldEnabledKey = QStringLiteral( "/%1/%2/%3/enabled" ).arg( mActiveProject->projectFullName() ).arg( layer->id() ).arg( fieldIndex );
  bool fieldEnabled = settings.value( fieldEnabledKey, false ).toBool();

  if ( layer && shouldRemember )
  {
    if ( !fieldEnabled )
      settings.setValue( fieldEnabledKey, true );

    settings.endGroup();
    return true;
  }

  if ( fieldEnabled )
    settings.setValue( fieldEnabledKey, false );

  settings.endGroup();
  return false;
}

bool RememberAttributesController::rememberedValue(
  const QgsVectorLayer *layer,
  int fieldIndex,
  QVariant &value ) const
{
  QSettings settings;
  settings.beginGroup( CACHED_ATTRIBUTES_GROUP );

  QString fieldEnabledKey = QStringLiteral( "/%1/%2/%3/enabled" ).arg( mActiveProject->projectFullName() ).arg( layer->id() ).arg( fieldIndex );
  bool fieldEnabled = settings.value( fieldEnabledKey, false ).toBool();

  if ( fieldEnabled )
  {
    QString fieldValueKey = QStringLiteral( "/%1/%2/%3/value" ).arg( mActiveProject->projectFullName() ).arg( layer->id() ).arg( fieldIndex );
    QVariant fieldValue = settings.value( fieldValueKey, QVariant() );
    value = fieldValue;
    settings.endGroup();
    return true;
  }

  settings.endGroup();
  return false;
}

FeatureLayerPair RememberAttributesController::featureLayerPair() const
{
  return mFeatureLayerPair;
}

void RememberAttributesController::setFeatureLayerPair( const FeatureLayerPair &pair )
{
  if ( mFeatureLayerPair != pair )
  {
    mFeatureLayerPair = pair;
    emit featureLayerPairChanged();
  }

  const QgsFeature &feature = pair.feature();
  const QgsVectorLayer *layer = pair.layer();

  if ( !layer || !mActiveProject )
    return;

  QSettings settings;
  settings.beginGroup( CACHED_ATTRIBUTES_GROUP );

  const QgsFields &fields = layer->fields();
  for ( int fieldIndex = 0; fieldIndex < fields.count(); fieldIndex++ )
  {
    QVariant value = feature.attribute( fieldIndex );
    QString fieldValueKey = QStringLiteral( "/%1/%2/%3/value" ).arg( mActiveProject->projectFullName() ).arg( layer->id() ).arg( fieldIndex );

    if ( shouldRememberValue( layer, fieldIndex ) )
    {
      settings.setValue( fieldValueKey, value );
    }
    else if ( layer->editFormConfig().reuseLastValue( ( fieldIndex ) ) )
    {
      setShouldRememberValue( layer, fieldIndex, true );
      settings.setValue( fieldValueKey, value );
    }
  }

  settings.endGroup();
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
