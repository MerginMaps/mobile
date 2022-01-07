/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "coreutils.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QTextStream>

#include "qcoreapplication.h"
#include "merginapi.h"

const QString CoreUtils::LOG_TO_DEVNULL = QStringLiteral();
const QString CoreUtils::LOG_TO_STDOUT = QStringLiteral( "TO_STDOUT" );
QString CoreUtils::sLogFile = CoreUtils::LOG_TO_DEVNULL;

QString CoreUtils::appInfo()
{
  return QString( "%1/%2 (%3/%4)" ).arg( QCoreApplication::applicationName() ).arg( QCoreApplication::applicationVersion() )
         .arg( QSysInfo::productType() ).arg( QSysInfo::productVersion() );
}

QString CoreUtils::appVersion()
{
  QString version;
#ifdef INPUT_VERSION
  version = STR( INPUT_VERSION );
#endif
  return version;
}


QString CoreUtils::localizedDateFromUTFString( QString timestamp )
{
  if ( timestamp.isEmpty() )
    return QString();

  QDateTime dateTime = QDateTime::fromString( timestamp, Qt::ISODate );
  if ( dateTime.isValid() )
  {
    return dateTime.date().toString( Qt::DefaultLocaleShortDate );
  }
  else
  {
    qDebug() << "Unable to convert UTF " << timestamp << " to QDateTime";
    return QString();
  }
}

QString CoreUtils::uuidWithoutBraces( const QUuid &uuid )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 11, 0 )
  return uuid.toString( QUuid::WithoutBraces );
#else
  QString str = uuid.toString();
  str = str.mid( 1, str.length() - 2 );  // remove braces
  return str;
#endif
}

bool CoreUtils::removeDir( const QString &dir )
{
  if ( dir.isEmpty() || dir == "/" )
    return false;

  return QDir( dir ).removeRecursively();
}

QString CoreUtils::downloadInProgressFilePath( const QString &projectDir )
{
  return projectDir + "/.mergin/.project.downloading";
}


void CoreUtils::setLogFilename( const QString &value )
{
  sLogFile = value;
}

QString CoreUtils::logFilename()
{
  return sLogFile;
}

void CoreUtils::log( const QString &topic, const QString &info )
{
  QString logFilePath;
  QByteArray data;
  data.append( QString( "%1 %2: %3\n" ).arg( QDateTime().currentDateTimeUtc().toString( Qt::ISODateWithMs ) ).arg( topic ).arg( info ) );
  appendLog( data, sLogFile );
}

void CoreUtils::appendLog( const QByteArray &data, const QString &path )
{
  if ( path != LOG_TO_DEVNULL )
  {
    if ( path == LOG_TO_STDOUT )
    {
      QTextStream out( stdout );
      out << data;
    }
    else
    {
      qDebug() << data;
      QFile file( path );
      if ( path.isEmpty() || !file.open( QIODevice::Append ) )
      {
        qDebug() << "ERROR: Invalid log file";
        return;
      }
      file.write( data );
      file.close();
    }
  }
}

QDateTime CoreUtils::getLastModifiedFileDateTime( const QString &path )
{
  QDateTime lastModified;
  QDirIterator it( path, QStringList() << QStringLiteral( "*" ), QDir::Files, QDirIterator::Subdirectories );
  while ( it.hasNext() )
  {
    it.next();
    if ( !MerginApi::isInIgnore( it.fileInfo() ) )
    {
      if ( it.fileInfo().lastModified() > lastModified )
      {
        lastModified = it.fileInfo().lastModified();
      }
    }
  }
  return lastModified.toUTC();
}

int CoreUtils::getProjectFilesCount( const QString &path )
{
  int count = 0;
  QDirIterator it( path, QStringList() << QStringLiteral( "*" ), QDir::Files, QDirIterator::Subdirectories );
  while ( it.hasNext() )
  {
    it.next();
    if ( !MerginApi::isInIgnore( it.fileInfo() ) )
    {
      count++;
    }
  }
  return count;
}

QString CoreUtils::findUniquePath( const QString &path )
{
  QFileInfo originalPath( path );
  QString uniquePath = path;

  // are we dealing with directory?
  bool isDirectory = originalPath.isDir();

  int i = 0;
  QFileInfo f( uniquePath );

  while ( f.exists() )
  {
    ++i; // let's start from 1

    if ( isDirectory )
    {
      uniquePath = path + " (" + QString::number( i ) + ')';
    }
    else // file
    {
      uniquePath = originalPath.path() + '/' + originalPath.baseName() + " (" + QString::number( i ) + ")." + originalPath.completeSuffix();
    }
    f.setFile( uniquePath );
  }

  return uniquePath;
}

QString CoreUtils::createUniqueProjectDirectory( const QString &baseDataDir, const QString &projectName )
{
  QString projectDirPath = findUniquePath( baseDataDir + "/" + projectName );
  QDir projectDir( projectDirPath );
  if ( !projectDir.exists() )
  {
    QDir dir( "" );
    dir.mkdir( projectDirPath );
  }
  return projectDirPath;
}

bool CoreUtils::createEmptyFile( const QString &filePath )
{
  QFile newFile( filePath );
  if ( !newFile.open( QIODevice::WriteOnly ) )
    return false;

  newFile.close();
  return true;
}

QString CoreUtils::generateConflictedCopyFileName( const QString &file, const QString &username, int version )
{
  if ( file.isEmpty() )
    return QString();

  QFileInfo f( file );

  return QString( "%1/%2 (conflicted copy, %3 v%4).%5" ).arg( f.path(), f.baseName(), username, QString::number( version ), f.completeSuffix() );
}

QString CoreUtils::generateEditConflictFileName( const QString &file, const QString &username, int version )
{
  if ( file.isEmpty() )
    return QString();

  QFileInfo f( file );
  return QString( "%1/%2 (edit conflict, %3 v%4).json" ).arg( f.path(), f.baseName(), username, QString::number( version ) );
}
