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
  int gpsTolerance = settings.value( "gpsTolerance", 10 ).toInt();
  int lineRecordingInterval = settings.value( "lineRecordingInterval", 3 ).toInt();
  bool reuseLastEnteredValues = settings.value( "reuseLastEnteredValues", false ).toBool();
  settings.endGroup();

  setDefaultProject( path );
  setActiveProject( path );
  setDefaultLayer( layer );
  setAutoCenterMapChecked( autoCenter );
  setGpsAccuracyTolerance( gpsTolerance );
  setLineRecordingInterval( lineRecordingInterval );
  setReuseLastEnteredValues( reuseLastEnteredValues );
}

QString AppSettings::defaultLayer() const
{
  return mDefaultLayers.value( mActiveProject );
}

void AppSettings::setDefaultLayer( const QString &value )
{
  if ( defaultLayer() != value )
  {
    QSettings settings;
    settings.beginGroup( mGroupName );
    settings.setValue( "defaultLayer/" + mActiveProject, value );
    settings.endGroup();
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
    QSettings settings;
    settings.beginGroup( mGroupName );
    settings.setValue( "defaultProject", value );
    settings.endGroup();

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
    QSettings settings;
    settings.beginGroup( mGroupName );
    settings.setValue( "autoCenter", value );
    settings.endGroup();

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

int AppSettings::gpsAccuracyTolerance() const
{
  return mGpsAccuracyTolerance;
}

void AppSettings::setGpsAccuracyTolerance( int value )
{
  if ( mGpsAccuracyTolerance != value )
  {
    mGpsAccuracyTolerance = value;
    QSettings settings;
    settings.beginGroup( mGroupName );
    settings.setValue( "gpsTolerance", value );
    settings.endGroup();

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
    QSettings settings;
    settings.beginGroup( mGroupName );
    settings.setValue( "lineRecordingInterval", value );
    settings.endGroup();

    emit lineRecordingIntervalChanged();
  }
}

bool AppSettings::reuseLastEnteredValues() const
{
  return mReuseLastEnteredValues;
}

void AppSettings::setReuseLastEnteredValues( bool reuseLastEnteredValues )
{
  if ( mReuseLastEnteredValues != reuseLastEnteredValues )
  {
    QSettings settings;
    settings.beginGroup( mGroupName );
    settings.setValue( "reuseLastEnteredValues", reuseLastEnteredValues );
    mReuseLastEnteredValues = reuseLastEnteredValues;
    emit reuseLastEnteredValuesChanged( mReuseLastEnteredValues );
  }
}
