/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "appsettings.h"

#include <QSettings>
#include <QFileInfo>

AppSettings::AppSettings( QObject *parent ): QObject( parent )
{
  QSettings settings;
  settings.beginGroup( mGroupName );
  QString path = settings.value( "defaultProject", "" ).toString();
  QString layer = settings.value( "defaultLayer/"  + path, "" ).toString();
  bool autoCenter = settings.value( "autoCenter", false ).toBool();
  double gpsTolerance = settings.value( "gpsTolerance", 10 ).toDouble();
  bool gpsAccuracyWarning = settings.value( "gpsAccuracyWarning", true ).toBool();
  int lineRecordingInterval = settings.value( "lineRecordingInterval", 3 ).toInt();
  bool reuseLastEnteredValues = settings.value( "reuseLastEnteredValues", false ).toBool();
  QString savedAppVersion = settings.value( QStringLiteral( "appVersion" ), QStringLiteral() ).toString();
  bool legacyFolderMigrated = settings.value( QStringLiteral( "legacyFolderMigrated" ), false ).toBool();
  settings.endGroup();

  setDefaultProject( path );
  setActiveProject( path );
  setDefaultLayer( layer );
  setAutoCenterMapChecked( autoCenter );
  setGpsAccuracyTolerance( gpsTolerance );
  setGpsAccuracyWarning( gpsAccuracyWarning );
  setLineRecordingInterval( lineRecordingInterval );
  setReuseLastEnteredValues( reuseLastEnteredValues );
  setAppVersion( savedAppVersion );
  setLegacyFolderMigrated( legacyFolderMigrated );
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

bool AppSettings::legacyFolderMigrated()
{
  return mLegacyFolderMigrated;
}

void AppSettings::setLegacyFolderMigrated( bool hasBeenMigrated )
{
  if ( mLegacyFolderMigrated == hasBeenMigrated )
    return;

  mLegacyFolderMigrated = hasBeenMigrated;
  setValue( QStringLiteral( "legacyFolderMigrated" ), hasBeenMigrated );
  emit legacyFolderMigratedChanged( mLegacyFolderMigrated );
}


void AppSettings::setValue( const QString &key, const QVariant &value )
{
  QSettings settings;
  settings.beginGroup( mGroupName );
  settings.setValue( key, value );
  settings.endGroup();
}

QVariant AppSettings::value( const QString &key, const QVariant &defaultValue )
{
  QSettings settings;
  settings.beginGroup( mGroupName );
  QVariant value = settings.value( key, defaultValue );
  settings.endGroup();

  return value;
}
