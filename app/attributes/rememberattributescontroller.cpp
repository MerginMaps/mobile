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
    QString fieldEnabledKey = keyForField( "enabled", layer, fieldIndex );
    bool fieldEnabled = settings.value( fieldEnabledKey, false ).toBool();
    bool qgisReuseLastValue = layer->editFormConfig().reuseLastValue( ( fieldIndex ) );

    if ( rememberValuesAllowed() ? fieldEnabled : qgisReuseLastValue ) //we want to keep QGIS's reuseLastValue independent when global switch off
    {
      QString fieldValueKey = keyForField( "value", layer, fieldIndex );
      QVariant value = feature.attribute( fieldIndex );
      settings.setValue( fieldValueKey, value );
    }
  }

  settings.endGroup();
}

bool RememberAttributesController::shouldRememberValue( const QgsVectorLayer *layer, int fieldIndex ) const
{
  if ( !layer )
    return false;

  QSettings settings;
  settings.beginGroup( CACHED_ATTRIBUTES_GROUP );

  QString fieldEnabledKey = keyForField( "enabled", layer, fieldIndex );
  bool currentlyEnabled = settings.value( fieldEnabledKey, layer->editFormConfig().reuseLastValue( fieldIndex ) ).toBool();

  settings.endGroup();

  return currentlyEnabled;
}

bool RememberAttributesController::setShouldRememberValue( const QgsVectorLayer *layer, int fieldIndex, bool shouldRemember )
{
  if ( !layer || !mActiveProject )
    return false;

  QSettings settings;
  settings.beginGroup( CACHED_ATTRIBUTES_GROUP );
  bool qgisReuseLastValue = layer->editFormConfig().reuseLastValue( ( fieldIndex ) );
  QString fieldEnabledKey = keyForField( "enabled", layer, fieldIndex );
  bool currentlyEnabled = settings.value( fieldEnabledKey, qgisReuseLastValue ).toBool();

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

  QString fieldEnabledKey = keyForField( "enabled", layer, fieldIndex );
  bool fieldEnabled = settings.value( fieldEnabledKey, false ).toBool();
  bool qgisReuseLastValue = layer->editFormConfig().reuseLastValue( ( fieldIndex ) );

  if ( rememberValuesAllowed() ? fieldEnabled : qgisReuseLastValue ) //we want to keep QGIS's reuseLastValue independent when global switch off
  {
    QString fieldValueKey = keyForField( "value", layer, fieldIndex );
    QVariant fieldValue = settings.value( fieldValueKey, QVariant() );
    value = fieldValue;
    settings.endGroup();
    return true;
  }

  settings.endGroup();
  return false;
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

QString RememberAttributesController::keyForField( const QString &suffix, const QgsVectorLayer *layer, int fieldIndex ) const
{
  return QStringLiteral( "/%1/%2/%3/%4" ).arg( mActiveProject->projectFullName(), layer->id(), QString::number( fieldIndex ), suffix );
}
