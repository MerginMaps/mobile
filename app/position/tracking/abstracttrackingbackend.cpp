/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "abstracttrackingbackend.h"

#include <QStandardPaths>
#include <QDir>
#include <QGuiApplication>

AbstractTrackingBackend::AbstractTrackingBackend(
  UpdateFrequency updateFrequency,
  QObject *parent
)
  : QObject( parent )
  , mUpdateFrequency( updateFrequency )
{
  QDir appData( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) );
  QString trackingFilePath = appData.absoluteFilePath( QStringLiteral( "tracking_data.txt" ) );

  qDebug() << "Position Tracking --> file path:" << trackingFilePath;

  // TODO: store tracking file in project data directory instead of standard location
  mFile.setFileName( trackingFilePath );
}

AbstractTrackingBackend::UpdateFrequency AbstractTrackingBackend::updateFrequency() const
{
  return mUpdateFrequency;
}

void AbstractTrackingBackend::setUpdateFrequency( const UpdateFrequency &newUpdateFrequency )
{
  if ( mUpdateFrequency == newUpdateFrequency )
    return;
  mUpdateFrequency = newUpdateFrequency;
  emit updateFrequencyChanged( mUpdateFrequency );
}

void AbstractTrackingBackend::storeDataAndNotify( double x, double y, double z, double m )
{
  //
  // TODO: keep the file opened
  //

  if ( !mFile.open( QFile::Append ) )
  {
    qDebug() << "Could not open tracking file when storing data!";
  }

  //
  // In the future we might want to store in binary to optimize the file size and read/write
  //

  QString trackline = QStringLiteral("%1 %2 %3 %4\n").arg(x).arg(y).arg(z).arg(m);
  mFile.write( trackline.toUtf8() );

  mFile.close();

  // now let's notify the manager

  // TODO: is it ok to use QGuiApplication here? It runs in android thread
  if ( QGuiApplication::applicationState() == Qt::ApplicationState::ApplicationActive )
  {
    emit positionUpdated();
  }
}
