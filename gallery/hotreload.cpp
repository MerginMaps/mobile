/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "hotreload.h"
#include <QFileSystemWatcher>
#include <QDir>
#include <QGuiApplication>
#include <QProcess>
#include <QTimer>

// TODO: not needed to sync dirs every second, just when a file was changed
QString HotReload::syncScript() const
{
  return "#!/bin/sh \n\
echo running hot reload sync directories ... \n\
while true; do \n\
  rsync -ra ../../../../gallery/qml/ HotReload/qml/ \n\
  rsync -ra ../../../../app/qml/ HotReload/app/qml/ \n\
  sleep 1 \n\
done";
}

HotReload::HotReload( QQmlApplicationEngine &engine, QObject *parent ):
  _engine( engine )
{
  // create dirs for sync (near the app)
  if ( !QDir( "HotReload/qml/" ).exists() )
    QDir().mkpath( QGuiApplication::applicationDirPath() + "/HotReload/qml/" );
  if ( !QDir( "HotReload/app/qml/" ).exists() )
    QDir().mkpath( QGuiApplication::applicationDirPath() + "/HotReload/app/qml/" );

  // create runnable sync script (near the app)
  QString scriptFilename = QGuiApplication::applicationDirPath() + "/syncGallery.sh";
  qInfo() << "Sync script location: " << scriptFilename;
  if ( !QFileInfo::exists( scriptFilename ) )
  {
    QFile file( QFileInfo( scriptFilename ).absoluteFilePath() );
    const QString script = syncScript();
    if ( !file.open( QIODevice::WriteOnly ) )
    {
      qInfo() << "Cannot create script file";
      exit( 1 );
    }
    QTextStream out( &file );
    out << script;
    file.close();
    QProcess::execute( "chmod", QStringList() << "+x" << file.fileName() );
  }

  // run sync script
  QProcess::startDetached( "./syncGallery.sh" );

  // start watching the changes in synced dirs
  QTimer::singleShot( 2000, this, &HotReload::startHotReload );
}

void HotReload::clearCache()
{
  _engine.clearComponentCache();
}

void HotReload::startHotReload()
{
  _watcher = new QFileSystemWatcher( this );
  _watcher->addPath( "HotReload/qml" );
  _watcher->addPath( "HotReload/qml/Pages" );
  _watcher->addPath( "HotReload/app/qml" );
  _watcher->addPath( "HotReload/app/qml/components" );
  _watcher->addPath( "HotReload/app/qml/form" );
  _watcher->addPath( "HotReload/app/qml/form/editors" );

  // send signal for hot reloading
  connect( _watcher, &QFileSystemWatcher::directoryChanged, this, [this]( const QString & path )
  {
    emit watchedSourceChanged();
  } );
}
