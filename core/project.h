/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QDateTime>
#include <QDirIterator>
#include <memory>
#include <qdebug.h>

struct Project;

namespace ProjectStatus
{
  Q_NAMESPACE
  enum Status
  {
    NoVersion,  //!< the project is not downloaded
    UpToDate,   //!< both server and local copy are in sync with no extra modifications
    OutOfDate,  //!< server has newer version than what is available locally (but the project is not modified locally)
    Modified    //!< there are some local modifications in the project that need to be pushed (note: also server may have newer version)
    // Maybe orphaned state in future
  };
  Q_ENUM_NS( Status )

  Status projectStatus( const std::shared_ptr<Project> project );
}

struct LocalProject
{
  LocalProject() {}; // TODO: define copy constructor
  ~LocalProject() {};

  QString projectName;
  QString projectNamespace;

  QString id() const; //! projectFullName for time being

  QString projectDir;
  QString projectError; // Error that leads to project not being able to open in app

  QString qgisProjectFilePath;

  int localVersion = -1;

  bool isValid() { return !projectDir.isEmpty(); }

  bool operator ==( const LocalProject &other )
  {
    return ( this->id() == other.id() );
  }

  bool operator !=( const LocalProject &other )
  {
    return !( *this == other );
  }
};

struct MerginProject
{
  MerginProject() {};
  ~MerginProject() {};

  QString projectName;
  QString projectNamespace;

  QString id() const; //! projectFullName for time being

  QDateTime serverUpdated; // available latest version of project files on server
  int serverVersion;

  ProjectStatus::Status status = ProjectStatus::NoVersion;
  bool pending = false;

  qreal progress = 0;

  // Maybe better use enum or int for error code
  QString remoteError; // Error leading to project not being able to sync

  bool isValid() const { return !projectName.isEmpty() && !projectNamespace.isEmpty(); }

  bool operator ==( const MerginProject &other )
  {
    return ( this->id() == other.id() );
  }

  bool operator !=( const MerginProject &other )
  {
    return !( *this == other );
  }
};

struct Project
{
  Project() {};
  ~Project() {};

  std::unique_ptr<MerginProject> mergin;
  std::unique_ptr<LocalProject> local;

  bool isMergin() const { return mergin != nullptr; }
  bool isLocal() const { return local != nullptr; }

  QString projectName()
  {
    if ( isMergin() ) return mergin->projectName;
    else if ( isLocal() ) return local->projectName;
    return QString();
  }

  QString projectNamespace()
  {
    if ( isMergin() ) return mergin->projectNamespace;
    else if ( isLocal() ) return local->projectNamespace;
    return QString();
  }

  QString projectId()
  {
    if ( isMergin() ) return mergin->id();
    else if ( isLocal() ) return local->id();
    return QString();
  }

  bool operator ==( const Project &other )
  {
    if ( this->isLocal() && other.isLocal() )
    {
      return this->local->id() == other.local->id();
    }
    else if ( this->isMergin() && other.isMergin() )
    {
      return this->mergin->id() == other.mergin->id();
    }
    return false;
  }

  bool operator !=( const Project &other )
  {
    return !( *this == other );
  }
};

typedef QList<MerginProject> MerginProjectsList;
typedef QList<LocalProject> LocalProjectsList;
Q_DECLARE_METATYPE( MerginProjectsList )
#endif // PROJECT_H
