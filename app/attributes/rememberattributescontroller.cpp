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

    bool qgisReuse = layer->editFormConfig().reuseLastValue( ( fieldIndex ) );

    bool con1 = (fieldEnabled && rememberValuesAllowed());
    bool con2 = ( !rememberValuesAllowed() && qgisReuse);

    if ( con1 || con2 )
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
  // // global switch off of the functionality

  if ( !rememberValuesAllowed() )
    return layer->editFormConfig().reuseLastValue( ( fieldIndex ) );

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
  if ( !layer || !mActiveProject )
    return false;

  QSettings settings;
  settings.beginGroup( CACHED_ATTRIBUTES_GROUP );

  bool qgisReuse = layer->editFormConfig().reuseLastValue( ( fieldIndex ) );

  bool currentlyEnabled;
  QString fieldEnabledKey = QStringLiteral( "/%1/%2/%3/enabled" ).arg( mActiveProject->projectFullName() ).arg( layer->id() ).arg( fieldIndex );

  if (settings.contains(fieldEnabledKey))
    currentlyEnabled = settings.value( fieldEnabledKey, false ).toBool();
  else if (qgisReuse)
    currentlyEnabled = qgisReuse;

  if ( currentlyEnabled != shouldRemember )
    settings.setValue( fieldEnabledKey, shouldRemember );

  settings.endGroup();
  return shouldRemember;
}

bool RememberAttributesController::rememberedValue(
  const QgsVectorLayer *layer,
  int fieldIndex,
  QVariant &value ) const
{
  if ( !layer || !mActiveProject )
    return false;

  QSettings settings;
  settings.beginGroup( CACHED_ATTRIBUTES_GROUP );

  QString fieldEnabledKey = QStringLiteral( "/%1/%2/%3/enabled" ).arg( mActiveProject->projectFullName() ).arg( layer->id() ).arg( fieldIndex );
  bool fieldEnabled = settings.value( fieldEnabledKey, false ).toBool();

  bool qgisReuse = layer->editFormConfig().reuseLastValue( ( fieldIndex ) );

  bool con1 = (fieldEnabled && rememberValuesAllowed());
  bool con2 = ( !rememberValuesAllowed() && qgisReuse);
  if ( con1 || con2 )
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
    if ( shouldRememberValue( layer, fieldIndex ) )
    {
      QString fieldValueKey = QStringLiteral( "/%1/%2/%3/value" ).arg( mActiveProject->projectFullName() ).arg( layer->id() ).arg( fieldIndex );
      QVariant rememberedValue = settings.value( fieldValueKey );
      mFeatureLayerPair.featureRef().setAttribute( fieldIndex, rememberedValue );
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
