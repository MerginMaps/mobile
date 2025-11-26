/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "coreutils.h"
#include "mmconfig.h"

#include <QDateTime>
#include <QDebug>
#include <QUuid>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QTextStream>
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QStorageInfo>
#include <QJsonDocument>
#include <QJsonObject>

#include "qcoreapplication.h"

const QString CoreUtils::QSETTINGS_APP_GROUP_NAME = QStringLiteral( "inputApp" );
const QString CoreUtils::LOG_TO_DEVNULL = QStringLiteral();
const QString CoreUtils::LOG_TO_STDOUT = QStringLiteral( "TO_STDOUT" );
QString CoreUtils::sLogFile = CoreUtils::LOG_TO_DEVNULL;
int CoreUtils::CHECKSUM_CHUNK_SIZE = 65536;

QString CoreUtils::deviceUuid()
{
  QString uuid;

  QSettings settings;
  settings.beginGroup( QSETTINGS_APP_GROUP_NAME );
  QVariant uuidEntry = settings.value( "deviceUuid" );
  if ( uuidEntry.isNull() )
  {
    uuid = uuidWithoutBraces( QUuid::createUuid() );
    CoreUtils::log( QStringLiteral( "Device" ), QStringLiteral( "deviceUuid generated: %1" ).arg( uuid ) );
    settings.setValue( "deviceUuid", uuid );
  }
  else
  {
    uuid = uuidEntry.toString();
  }
  settings.endGroup();

  Q_ASSERT( !uuid.isEmpty() );
  return uuid;
}

QString CoreUtils::appInfo()
{
  return QString( "%1/%2 (%3/%4)" ).arg( QCoreApplication::applicationName() ).arg( QCoreApplication::applicationVersion() )
         .arg( QSysInfo::productType() ).arg( QSysInfo::productVersion() );
}

QString CoreUtils::appVersion()
{
  QString version;
#ifdef MM_VERSION
  version = STR( MM_VERSION );
#endif
  return version;
}

QString CoreUtils::appVersionCode()
{
  QString version;
#ifdef MM_VERSION_CODE
  version = STR( MM_VERSION_CODE );
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
    QLocale locale = QLocale::system();
    return locale.toString( dateTime.date(), locale.dateFormat( QLocale::ShortFormat ) );
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
  data.append( QString( "%1 %2: %3\n" ).arg( QDateTime().currentDateTimeUtc().toString( Qt::ISODateWithMs ) ).arg( topic ).arg( info ).toUtf8() );
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

QByteArray CoreUtils::calculateChecksum( const QString &filePath )
{
  QFile f( filePath );
  if ( f.open( QFile::ReadOnly ) )
  {
    QCryptographicHash hash( QCryptographicHash::Sha1 );
    QByteArray chunk = f.read( CHECKSUM_CHUNK_SIZE );
    while ( !chunk.isEmpty() )
    {
      hash.addData( chunk );
      chunk = f.read( CHECKSUM_CHUNK_SIZE );
    }
    f.close();
    return hash.result().toHex();
  }

  return QByteArray();
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

  QString suffix = f.completeSuffix();
  if ( hasProjectFileExtension( file ) )
  {
    suffix += "~";
  }
  return QString( "%1/%2 (conflicted copy, %3 v%4).%5" ).arg( f.path(), f.baseName(), username, QString::number( version ).toUtf8(), suffix );
}

QString CoreUtils::generateEditConflictFileName( const QString &file, const QString &username, int version )
{
  if ( file.isEmpty() )
    return QString();

  QFileInfo f( file );
  return QString( "%1/%2 (edit conflict, %3 v%4).json" ).arg( f.path(), f.baseName(), username, QString::number( version ) );
}

bool CoreUtils::hasProjectFileExtension( const QString filePath )
{
  return filePath.contains( ".qgs", Qt::CaseInsensitive ) || filePath.contains( ".qgz", Qt::CaseInsensitive );
}

bool CoreUtils::isValidName( const QString &name )
{
  static QRegularExpression reForbiddenmNames( R"([@#$%^&*\(\)\{\}\[\]\\\/\|\+=<>~\?:;,`\'\"]|^[\s^\.].*$|^CON$|^PRN$|^AUX$|^NUL$|^COM\d$|^LPT\d|^support$|^helpdesk$|^merginmaps$|^lutraconsulting$|^mergin$|^lutra$|^input$|^sales$|^admin$)", QRegularExpression::CaseInsensitiveOption );
  QRegularExpressionMatch matchForbiddenNames = reForbiddenmNames.match( name );
  return !matchForbiddenNames.hasMatch();
}

QString CoreUtils::nameAbbr( const QString &name, const QString &email )
{
  if ( name.isEmpty() )
    return email.left( 2 ).toUpper();

  static QRegularExpression re( R"([\r\n\t ]+)" );
  QStringList list = name.split( re, Qt::SplitBehaviorFlags::SkipEmptyParts );

  if ( list.size() > 1 )
    return QString( "%1%2" ).arg( list.first()[0], list.last()[0] ).toUpper();

  if ( email.isEmpty() )
    return name.left( 2 ).toUpper();

  return email.left( 2 ).toUpper();
}

QString CoreUtils::getAvailableDeviceStorage()
{
  QString appDir = QCoreApplication::applicationDirPath();
  QStorageInfo storageInfo( appDir );

  if ( storageInfo.isValid() && storageInfo.isReady() )
  {
    return bytesToHumanSize( storageInfo.bytesAvailable() );
  }

  return "N/A";
}

QString CoreUtils::getTotalDeviceStorage()
{
  QString appDir = QCoreApplication::applicationDirPath();
  QStorageInfo storageInfo( appDir );

  if ( storageInfo.isValid() && storageInfo.isReady() )
  {
    return bytesToHumanSize( storageInfo.bytesTotal() );
  }

  return "N/A";
}

QString CoreUtils::bytesToHumanSize( double bytes )
{
  const int precision = 1;
  if ( bytes < 1e-5 )
  {
    return "0.0";
  }
  else if ( bytes < 1024.0 * 1024.0 )
  {
    return QString::number( bytes / 1024.0, 'f', precision ) + " KB";
  }
  else if ( bytes < 1024.0 * 1024.0 * 1024.0 )
  {
    return QString::number( bytes / 1024.0 / 1024.0, 'f', precision ) + " MB";
  }
  else if ( bytes < 1024.0 * 1024.0 * 1024.0 * 1024.0 )
  {
    return QString::number( bytes / 1024.0 / 1024.0 / 1024.0, 'f', precision ) + " GB";
  }
  else
  {
    return QString::number( bytes / 1024.0 / 1024.0 / 1024.0 / 1024.0, 'f', precision ) + " TB";
  }
}

QString CoreUtils::getProjectMetadataPath( QString projectDir )
{
  if ( projectDir.isEmpty() )
    return QString();

  return projectDir + "/.mergin/mergin.json";
}

bool CoreUtils::replaceValueInJson( const QString &filePath, const QString &key, const QJsonValue &value )
{
  QFile file( filePath );
  if ( !file.open( QIODevice::ReadOnly ) )
  {
    return false;
  }

  QByteArray data = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson( data );
  if ( !doc.isObject() )
  {
    return false;
  }

  QJsonObject obj = doc.object();
  obj[key] = value;
  doc.setObject( obj );

  if ( !file.open( QIODevice::WriteOnly ) )
  {
    return false;
  }

  bool success = ( file.write( doc.toJson() ) != -1 );
  file.close();

  return success;
}

bool CoreUtils::isValidEmail( const QString &email )
{
  const thread_local QRegularExpression regEx( "\\S+@\\S+\\.\\S+" );
  return regEx.match( email ).hasMatch();
}
