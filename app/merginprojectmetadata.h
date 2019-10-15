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

  //
  // these are members only used for upload of changed file through a geo-diff
  //

  QString diffName;          //!< if non-empty, we are uploading diff for this file and this is filename (relative to project's .mergin dir)
  QString diffChecksum;      //!< checksum for the diff file
  QString diffBaseChecksum;  //!< checksum of the base file
  qint64 diffSize;           //!< size (in bytes) of the diff file

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
