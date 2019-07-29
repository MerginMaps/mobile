#ifndef MERGINPROJECTMETADATA_H
#define MERGINPROJECTMETADATA_H

#include <QDateTime>
#include <QList>


struct MerginFile
{
  QString path;
  QString checksum;
  qint64 size;
  QDateTime mtime;
  QStringList chunks; // used only for upload otherwise suppose to be empty

  static MerginFile fromJsonObject( const QJsonObject &merginFileInfo );
};


//! Metadata read from project info reply or read from cached local .mergin.json file
struct MerginProjectMetadata
{
  QString name;
  QString projectNamespace;

  int version = -1;
  QList<MerginFile> files;

  // no project dir, no sync state, ...

  bool isValid() const { return !name.isEmpty() && !projectNamespace.isEmpty(); }

  static MerginProjectMetadata fromJson( const QByteArray &data );

  static MerginProjectMetadata fromCachedJson( const QString &metadataFilePath );

  MerginFile fileInfo( const QString &filePath ) const;
};


#endif // MERGINPROJECTMETADATA_H
