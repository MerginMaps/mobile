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

  //! Returns project state from ProjectStatus::Status enum for the project
  Status projectStatus( const std::shared_ptr<Project> project );
}

/**
 * \brief The LocalProject struct is used as a struct for projects that are available on the device.
 * The struct is used in the \see Projects struct and also for communication between LocalProjectsManager and ProjectsModel
 *
 * \note Struct contains member id() which in this time returns projects full name, however, once we
 * start using projects IDs, it can be replaced for that ID.
 */
struct LocalProject
{
  LocalProject() {};
  ~LocalProject() {};

  QString projectName;
  QString projectNamespace;

  QString id() const; //! projectFullName for time being

  QString projectDir;
  QString projectError; // Error that leads to project not being able to open in app

  QString qgisProjectFilePath;

  int localVersion = -1;

  bool isValid() { return !projectDir.isEmpty(); }

  LocalProject *clone() const;

  bool operator ==( const LocalProject &other )
  {
    return ( this->id() == other.id() );
  }

  bool operator !=( const LocalProject &other )
  {
    return !( *this == other );
  }
};

/**
 * \brief The MerginProject struct is used for projects that comes from Mergin.
 * This struct is used in the \see Projects struct and also for communication between MerginAPI and ProjectsModel
 *
 * \note Struct contains member id() which in this time returns projects full name, however, once we
 * start using projects IDs, it can be replaced for that ID.
 */
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

  QString remoteError; // Error leading to project not being able to sync (received error code from server)

  bool isValid() const { return !projectName.isEmpty() && !projectNamespace.isEmpty(); }

  MerginProject *clone() const;

  bool operator ==( const MerginProject &other )
  {
    return ( this->id() == other.id() );
  }

  bool operator !=( const MerginProject &other )
  {
    return !( *this == other );
  }
};

/**
 * \brief The Project struct serves as a struct for any kind of project (local/mergin).
 * It consists of two main parts - mergin and local.
 * Both parts are pointers to their specific structs and based on the pointer value (nullptr or assigned) this structs
 * decides if the project is local, mergin or both.
 */
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

  QString projectFullName()
  {
    return projectId();
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
