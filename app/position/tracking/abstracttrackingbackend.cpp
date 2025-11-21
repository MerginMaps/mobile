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
  QReadWriteLock *fileLock,
  TrackingUtils::UpdateFrequency updateFrequency,
  QObject *parent
)
  : QObject( parent )
  , mFileLock( fileLock )
  , mUpdateFrequency( updateFrequency )
{
  QDir appData( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) );
  QString trackingFilePath = appData.absoluteFilePath( QStringLiteral( "tracking_data.txt" ) );

  qDebug() << "Position Tracking --> file path:" << trackingFilePath;

  // TODO: store tracking file in project data directory instead of standard location
  mFile.setFileName( trackingFilePath );

  if ( !mFileLock )
  {
    qCritical() << "Error, received invalid file lock!";
  }
}

AbstractTrackingBackend::~AbstractTrackingBackend()
{
  if ( mFile.isOpen() )
  {
    mFile.close();
  }
}

void AbstractTrackingBackend::storeDataAndNotify( double x, double y, double z, double m )
{
  //
  // In the future we might want to store data in binary to optimize the file size and read/write
  //

  if ( !mFile.isOpen() )
  {
    if ( !mFile.open( QFile::Append ) )
    {
      qCritical() << "Error, could not open tracking file for writing!";
      return;
    }
  }

  if ( !mFileLock )
  {
    qCritical() << "Error, file lock is invalid!";
    return;
  }

  qDebug() << "--> W: Using file lock" << mFileLock;

  QString trackline = QStringLiteral("%1 %2 %3 %4\n").arg(x).arg(y).arg(z).arg(m);

  // QWriteLocker unlocks the lock once it goes out of scope
  {
    QWriteLocker writeLocker( mFileLock );

    mFile.write( trackline.toUtf8() );
  }

  // now let's notify the manager

  // TODO: is it ok to use QGuiApplication here? It runs in android thread
  if ( QGuiApplication::applicationState() == Qt::ApplicationState::ApplicationActive )
  {
    emit positionUpdated();
  }
}
