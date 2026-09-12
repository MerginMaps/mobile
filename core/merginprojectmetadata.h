/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MERGINPROJECTMETADATA_H
#define MERGINPROJECTMETADATA_H

#include <QDateTime>
#include <QList>
#include <QJsonObject>

struct FileChunkV2
{
  // struct used in push v2 when server responds after chunk upload
  QString id; // chunk id, as generated on server
  QDateTime valid_until; // chunk can be reused and not reuploaded until this datetime
};

struct MerginFile
{
  QString path;
  QString checksum;
  qint64 size;
  QDateTime mtime;

  QStringList chunks; // pregenerated chunks to upload; only used in v1

  int chunksCountV2; // contains the number of chunks this file should be split to for upload; only used in v2 push
  QList<FileChunkV2> chunksV2; // contains the list of uploaded chunks; only used in v2

  //
  // these are members only used for upload of changed file through a geo-diff
  //

  QString diffName;          //!< if non-empty, we are uploading diff for this file and this is filename (relative to project's .mergin dir)
  QString diffChecksum;      //!< checksum for the diff file
  QString diffBaseChecksum;  //!< checksum of the base file
  qint64 diffSize;           //!< size (in bytes) of the diff file

  //
  // these are members only user for update of changed file through geodiff
  // (could be multiple diffs that need to be applied sequentially)
  //

  bool pullCanUseDiff = false;  //!< whether or not we can update the local file by downloading and applying diffs
  QList< QPair<int, qint64> > pullDiffFiles;   //!< list of diffs that will need to be fetched: the version and their sizes

  static MerginFile fromJsonObject( const QJsonObject &merginFileInfo );
};

//! MerginConfig stored in .mergin-config.json, describing optional sync behavior
struct MerginConfig
{
  bool selectiveSyncEnabled = false;
  QString selectiveSyncDir;
  bool isValid = false;
  bool downloadMissingFiles = false; //!< indicates that this sync must download all files that are missing (excluding selective dir), because config was removed/changed

  static MerginConfig fromJson( const QByteArray &data );
  static MerginConfig fromFile( const QString &projectDir );
};

//! Metadata read from project info reply or read from cached local .mergin.json file
struct MerginProjectMetadata
{
  QString name;
  QString projectNamespace;
  QString role;
  int version = -1;
  QList<MerginFile> files;
  QString projectId; //!< unique project ID (only available in API that supports project IDs)

  // no project dir, no sync state, ...

  bool isValid() const { return !name.isEmpty() && !projectNamespace.isEmpty(); }

  static MerginProjectMetadata fromJson( const QByteArray &data );

  static MerginProjectMetadata fromCachedJson( const QString &metadataFilePath );

  MerginFile fileInfo( const QString &filePath ) const;
};


#endif // MERGINPROJECTMETADATA_H
