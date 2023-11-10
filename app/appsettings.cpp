/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "appsettings.h"
#include "coreutils.h"

#include <QSettings>
#include <QFileInfo>

const QString AppSettings::INPUTAPP_GROUP_NAME = QStringLiteral( "inputApp" );
const QString AppSettings::POSITION_PROVIDERS_GROUP = QStringLiteral( "inputApp/positionProviders" );
const int AppSettings::WS_TOOLTIP_MAX_NUM_OF_OCCURENCIES = 5;

AppSettings::AppSettings( QObject *parent ): QObject( parent )
{
  QSettings settings;
  settings.beginGroup( INPUTAPP_GROUP_NAME );
  QString path = settings.value( "defaultProject", "" ).toString();
  QString layer = settings.value( "defaultLayer/"  + path, "" ).toString();
  bool autoCenter = settings.value( "autoCenter", false ).toBool();
  double gpsTolerance = settings.value( "gpsTolerance", 10 ).toDouble();
  bool gpsAccuracyWarning = settings.value( "gpsAccuracyWarning", true ).toBool();
  int lineRecordingInterval = settings.value( "lineRecordingInterval", 3 ).toInt();
  int streamingIntervalType = settings.value( "intervalType", 0 ).toInt();
  StreamingIntervalType::IntervalType intervalType = static_cast<StreamingIntervalType::IntervalType>( streamingIntervalType );
  bool reuseLastEnteredValues = settings.value( "reuseLastEnteredValues", false ).toBool();
  QString savedAppVersion = settings.value( QStringLiteral( "appVersion" ), QStringLiteral() ).toString();
  QString activeProviderId = settings.value( QStringLiteral( "activePositionProviderId" ) ).toString();
  bool autosync = settings.value( QStringLiteral( "autosyncAllowed" ), false ).toBool();
  mWsTooltipShownCounter = settings.value( QStringLiteral( "wsTooltipCounter" ) ).toInt();
  double gpsHeight = settings.value( "gpsHeight", 0 ).toDouble();
  QString ignoreMigrateVersion = settings.value( QStringLiteral( "ignoreMigrateVersion" ) ).toString();

  settings.endGroup();

  setDefaultProject( path );
  setActiveProject( path );
  setDefaultLayer( layer );
  setAutoCenterMapChecked( autoCenter );
  setGpsAccuracyTolerance( gpsTolerance );
  setGpsAccuracyWarning( gpsAccuracyWarning );
  setLineRecordingInterval( lineRecordingInterval );
  setIntervalType( intervalType );
  setReuseLastEnteredValues( reuseLastEnteredValues );
  setAppVersion( savedAppVersion );
  setActivePositionProviderId( activeProviderId );
  setAutosyncAllowed( autosync );
  setGpsAntennaHeight( gpsHeight );
  setIgnoreMigrateVersion( ignoreMigrateVersion );
}

QString AppSettings::defaultLayer() const
{
  return mDefaultLayers.value( mActiveProject );
}

void AppSettings::setDefaultLayer( const QString &value )
{
  if ( defaultLayer() != value )
  {
    setValue( "defaultLayer/" + mActiveProject, value );
    mDefaultLayers.insert( mActiveProject, value );
    emit defaultLayerChanged();
  }
}

QString AppSettings::defaultProject() const
{
  return mDefaultProject;
}

void AppSettings::setDefaultProject( const QString &value )
{
  if ( mDefaultProject != value )
  {
    mDefaultProject = value;
    setValue( "defaultProject", value );

    emit defaultProjectChanged();
  }
}

QString AppSettings::activeProject() const
{
  return mActiveProject;
}

void AppSettings::setActiveProject( const QString &value )
{
  if ( mActiveProject != value )
  {
    mActiveProject = value;

    emit activeProjectChanged();
  }
}

bool AppSettings::autoCenterMapChecked()
{
  return mAutoCenterMapChecked;
}


void AppSettings::setAutoCenterMapChecked( bool value )
{

  if ( mAutoCenterMapChecked != value )
  {
    mAutoCenterMapChecked = value;
    setValue( "autoCenter", value );

    emit autoCenterMapCheckedChanged();
  }

}

QString AppSettings::defaultProjectName() const
{
  if ( !mDefaultProject.isEmpty() )
  {
    QFileInfo info( mDefaultProject );
    return info.baseName();
  }
  return QString( "" );
}

double AppSettings::gpsAccuracyTolerance() const
{
  return mGpsAccuracyTolerance;
}

void AppSettings::setGpsAccuracyTolerance( double value )
{
  if ( mGpsAccuracyTolerance != value )
  {
    mGpsAccuracyTolerance = value;
    setValue( "gpsTolerance", value );

    emit gpsAccuracyToleranceChanged();
  }
}

int AppSettings::lineRecordingInterval() const
{
  return mLineRecordingInterval;
}

void AppSettings::setLineRecordingInterval( int value )
{
  if ( mLineRecordingInterval != value )
  {
    mLineRecordingInterval = value;
    setValue( "lineRecordingInterval", value );

    emit lineRecordingIntervalChanged();
  }
}

StreamingIntervalType::IntervalType AppSettings::intervalType() const
{
  return mIntervalType;
}

void AppSettings::setIntervalType( StreamingIntervalType::IntervalType intervalType )
{
  if ( mIntervalType != intervalType )
  {
    mIntervalType = intervalType;
    setValue( "intervalType", intervalType );

    emit intervalTypeChanged();
  }
}

bool AppSettings::demoProjectsCopied()
{
  return value( QStringLiteral( "demoProjectsCopied" ), QVariant( false ) ).toBool();
}

void AppSettings::setDemoProjectsCopied( const bool value )
{
  setValue( "demoProjectsCopied", value );
}

bool AppSettings::reuseLastEnteredValues() const
{
  return mReuseLastEnteredValues;
}

void AppSettings::setReuseLastEnteredValues( bool reuseLastEnteredValues )
{
  if ( mReuseLastEnteredValues != reuseLastEnteredValues )
  {
    setValue( "reuseLastEnteredValues", reuseLastEnteredValues );
    mReuseLastEnteredValues = reuseLastEnteredValues;
    emit reuseLastEnteredValuesChanged( mReuseLastEnteredValues );
  }
}

bool AppSettings::gpsAccuracyWarning() const
{
  return mGpsAccuracyWarning;
}

void AppSettings::setGpsAccuracyWarning( bool gpsAccuracyWarning )
{
  if ( mGpsAccuracyWarning != gpsAccuracyWarning )
  {
    mGpsAccuracyWarning = gpsAccuracyWarning;
    setValue( "gpsAccuracyWarning", gpsAccuracyWarning );
    emit gpsAccuracyWarningChanged();
  }
}

QString AppSettings::appVersion() const
{
  return mAppVersion;
}

void AppSettings::setAppVersion( const QString &newAppVersion )
{
  if ( mAppVersion == newAppVersion )
    return;

  mAppVersion = newAppVersion;
  setValue( QStringLiteral( "appVersion" ), newAppVersion );
  emit appVersionChanged( mAppVersion );
}

const QString &AppSettings::activePositionProviderId() const
{
  return mActivePositionProviderId;
}

void AppSettings::setActivePositionProviderId( const QString &id )
{
  if ( mActivePositionProviderId == id )
    return;

  mActivePositionProviderId = id;
  setValue( QStringLiteral( "activePositionProviderId" ), id );
  emit activePositionProviderIdChanged( mActivePositionProviderId );
}

QVariantList AppSettings::savedPositionProviders() const
{
  QSettings settings;
  QVariantList providers;

  int size = settings.beginReadArray( POSITION_PROVIDERS_GROUP );

  for ( int i = 0; i < size; i++ )
  {
    settings.setArrayIndex( i );
    QStringList provider;
    provider << settings.value( "providerName" ).toString();
    provider << settings.value( "providerAddress" ).toString();
    providers.push_back( provider );
  }

  settings.endArray();

  return providers;
}

void AppSettings::savePositionProviders( const QVariantList &providers )
{
  QSettings settings;

  if ( settings.contains( POSITION_PROVIDERS_GROUP ) )
  {
    settings.remove( POSITION_PROVIDERS_GROUP );
  }

  settings.beginWriteArray( POSITION_PROVIDERS_GROUP );

  for ( int i = 0; i < providers.count(); i++ )
  {
    QVariantList provider = providers[i].toList();

    if ( provider.length() < 2 )
    {
      CoreUtils::log( QStringLiteral( "AppSettings" ), QStringLiteral( "Tried to save provider without sufficient data" ) );
      continue;
    }
    settings.setArrayIndex( i );

    settings.setValue( "providerName", providers[i].toList()[0] );
    settings.setValue( "providerAddress", providers[i].toList()[1] );
  }
  settings.endArray();
}

void AppSettings::setValue( const QString &key, const QVariant &value )
{
  QSettings settings;
  settings.beginGroup( INPUTAPP_GROUP_NAME );
  settings.setValue( key, value );
  settings.endGroup();
}

QVariant AppSettings::value( const QString &key, const QVariant &defaultValue )
{
  QSettings settings;
  settings.beginGroup( INPUTAPP_GROUP_NAME );
  QVariant value = settings.value( key, defaultValue );
  settings.endGroup();

  return value;
}

bool AppSettings::autosyncAllowed() const
{
  return mAutosyncAllowed;
}

void AppSettings::setAutosyncAllowed( bool newAutosyncAllowed )
{
  if ( mAutosyncAllowed == newAutosyncAllowed )
    return;

  mAutosyncAllowed = newAutosyncAllowed;
  setValue( QStringLiteral( "autosyncAllowed" ), newAutosyncAllowed );
  emit autosyncAllowedChanged( mAutosyncAllowed );
}

void AppSettings::wsTooltipShown()
{
  if ( mWsTooltipShownCounter < WS_TOOLTIP_MAX_NUM_OF_OCCURENCIES )
  {
    mWsTooltipShownCounter += 1;
    setValue( QStringLiteral( "wsTooltipCounter" ), mWsTooltipShownCounter );

    emit ignoreWsTooltipChanged();
  }
}

bool AppSettings::ignoreWsTooltip() const
{
  return mWsTooltipShownCounter >= WS_TOOLTIP_MAX_NUM_OF_OCCURENCIES;
}

double AppSettings::gpsAntennaHeight() const
{
  return mGpsAntennaHeight;
}

void AppSettings::setGpsAntennaHeight( double gpsAntennaHeight )
{
  double height = gpsAntennaHeight;
  if ( height <= 0 )
  {
    height = 0;
  }
  else
  {
    // keep 3 decimal places
    height = std::round( gpsAntennaHeight / 0.001 ) * 0.001;
  }
  if ( mGpsAntennaHeight != height )
  {
    mGpsAntennaHeight = gpsAntennaHeight;
    setValue( "gpsHeight", height );

    emit gpsAntennaHeightChanged( gpsAntennaHeight );
  }
}

QString AppSettings::ignoreMigrateVersion() const
{
  return mIgnoreMigrateVersion;
}

void AppSettings::setIgnoreMigrateVersion( const QString &version )
{
  if ( mIgnoreMigrateVersion == version )
    return;

  mIgnoreMigrateVersion = version;
  setValue( QStringLiteral( "ignoreMigrateVersion" ), version );
  emit ignoreMigrateVersionChanged();
}
