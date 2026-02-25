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

QString HotReload::syncScript() const
{
  return QString( "#!/bin/sh \n"
                  "echo 'Syncing modified files...' \n"
                  "rsync -rau \"%1/qml/\" HotReload/qml/ \n"
                  "rsync -rau \"%1/../app/qml/\" HotReload/app/qml/ \n" )
         .arg( GALLERY_SOURCE_DIR );
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
  _debounceTimer = new QTimer( this );
  _debounceTimer->setSingleShot( true );
  _debounceTimer->setInterval( 300 );

  // when the timer starts, run the sync script ONCE, then reload
  connect( _debounceTimer, &QTimer::timeout, this, [this]()
  {
    // run the sync synchronously so it finishes before reloading
    QProcess::execute( "./syncGallery.sh" );
    emit watchedSourceChanged();
  } );

  _watcher = new QFileSystemWatcher( this );

  // Set up base paths for your source code
  QString gallerySrc = QString( GALLERY_SOURCE_DIR ) + "/qml";
  QString appSrc = QString( GALLERY_SOURCE_DIR ) + "/../app/qml";

  // Watch the SOURCE directories instead of the destination
  _watcher->addPath( gallerySrc );
  _watcher->addPath( gallerySrc + "/Pages" );

  _watcher->addPath( appSrc + "/account" );
  _watcher->addPath( appSrc + "/account/components" );
  _watcher->addPath( appSrc + "/components" );
  _watcher->addPath( appSrc + "/dialogs" );
  _watcher->addPath( appSrc + "/form" );
  _watcher->addPath( appSrc + "/form/components" );
  _watcher->addPath( appSrc + "/form/editors" );
  _watcher->addPath( appSrc + "/gps" );
  _watcher->addPath( appSrc + "/inputs" );
  _watcher->addPath( appSrc + "/layers" );
  _watcher->addPath( appSrc + "/map" );
  _watcher->addPath( appSrc + "/project" );
  _watcher->addPath( appSrc + "/project/components" );
  _watcher->addPath( appSrc + "/settings" );
  _watcher->addPath( appSrc + "/settings/components" );

  // when you save the file, start the debounce timer
  connect( _watcher, &QFileSystemWatcher::directoryChanged, this, [this]( const QString & path )
  {
    _debounceTimer->start();
  } );

  connect( _watcher, &QFileSystemWatcher::fileChanged, this, [this]( const QString & path )
  {
    _debounceTimer->start();
  } );
}
