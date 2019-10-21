#include "merginprojectmetadata.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>



MerginFile MerginFile::fromJsonObject( const QJsonObject &merginFileInfo )
{
  MerginFile merginFile;
  merginFile.checksum = merginFileInfo.value( QStringLiteral( "checksum" ) ).toString();
  merginFile.path = merginFileInfo.value( QStringLiteral( "path" ) ).toString();
  merginFile.size = merginFileInfo.value( QStringLiteral( "size" ) ).toInt();
  merginFile.mtime =  QDateTime::fromString( merginFileInfo.value( QStringLiteral( "mtime" ) ).toString(), Qt::ISODateWithMs ).toUTC();

  if ( merginFileInfo.contains( QStringLiteral( "history" ) ) )
  {
    QJsonObject history = merginFileInfo.value( QStringLiteral( "history" ) ).toObject();
    QList<int> versions;
    for ( QString key : history.keys() )
      versions << key.mid( 1 ).toInt();
    qSort( versions );
    // remove the first version (which we already have)
    Q_ASSERT( versions.count() > 0 );
    versions.removeAt( 0 );
    for ( int key : versions )
    {
      QJsonObject obj = history.value( QString( "v%1" ).arg( key ) ).toObject();
      QJsonObject diffObj = obj["diff"].toObject();
      QString diffFile = diffObj["path"].toString();
      int fileSize = diffObj["size"].toInt();
      merginFile.diffFilesToFetch << qMakePair( diffFile, fileSize );
    }
  }

  return merginFile;
}


MerginProjectMetadata MerginProjectMetadata::fromJson( const QByteArray &data )
{
  MerginProjectMetadata project;

  QJsonDocument doc = QJsonDocument::fromJson( data );
  if ( !doc.isObject() )
  {
    qDebug() << "MerginProjectMetadata::fromJson: invalid content!";
    return project;
  }

  QJsonObject docObj = doc.object();

  // read metadata about project files
  QJsonValue vFiles = docObj.value( QStringLiteral( "files" ) );
  Q_ASSERT( vFiles.isArray() );
  QJsonArray vFilesArray = vFiles.toArray();
  for ( auto it = vFilesArray.constBegin(); it != vFilesArray.constEnd(); ++it )
  {
    project.files << MerginFile::fromJsonObject( it->toObject() );
  }

  project.name = docObj.value( QStringLiteral( "name" ) ).toString();
  project.projectNamespace = docObj.value( QStringLiteral( "namespace" ) ).toString();

  QString versionStr = docObj.value( QStringLiteral( "version" ) ).toString();
  if ( versionStr.isEmpty() )
  {
    project.version = 0;
  }
  else if ( versionStr.startsWith( "v" ) ) // cut off 'v' part from v123
  {
    versionStr = versionStr.mid( 1 );
    project.version = versionStr.toInt();
  }

  return project;
}

MerginProjectMetadata MerginProjectMetadata::fromCachedJson( const QString &metadataFilePath )
{
  QFile file( metadataFilePath );
  if ( file.open( QIODevice::ReadOnly ) )
  {
    return fromJson( file.readAll() );
  }
  return MerginProjectMetadata();
}

MerginFile MerginProjectMetadata::fileInfo( const QString &filePath ) const
{
  for ( const MerginFile &merginFile : files )
  {
    if ( merginFile.path == filePath )
      return merginFile;
  }
  qDebug() << "requested fileInfo() for non-existant file! " << filePath;
  return MerginFile();
}
