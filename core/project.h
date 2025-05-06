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

#include <QDirIterator>
#include <memory>

struct Project;
struct LocalProject;

namespace ProjectStatus
{
  Q_NAMESPACE
  enum Status
  {
    NoVersion,  //!< the project is not downloaded
    UpToDate,   //!< both server and local copy are in sync with no extra modifications
    NeedsSync,  //!< server has newer version than what is available locally and/or the project is modified locally
  };
  Q_ENUM_NS( Status )

  //! Returns project state from ProjectStatus::Status enum for the project
  Status projectStatus( const Project &project, bool supportsSelectiveSync );

  bool hasLocalChanges( const LocalProject &project, bool supportsSelectiveSync );
}

/**
 * \brief The LocalProject struct is used as a struct for projects that are available on the device.
 * The struct is used in the \see Projects struct and also for communication between LocalProjectsManager
 * and ProjectsModel.
 */
struct LocalProject
{
    Q_GADGET

  public:

    // TODO: remove?
    Q_PROPERTY( QString qgisProjectFilePath MEMBER qgisProjectFilePath )

    LocalProject() = default;
    ~LocalProject() = default;

    QString projectName;
    QString projectNamespace;
    QString projectId;

    //! Returns the UUID of project
    Q_INVOKABLE QString id() const;

    //! generates a new UUID
    static QString generateProjectId();

    QString fullName() const;

    QString projectDir;
    QString projectError; // Error that leads to project not being able to open in app

    QString qgisProjectFilePath;

    int localVersion = -1;

    bool isValid() const { return !projectDir.isEmpty(); }

    /**
     * Returns true if the local version instance has a mergin counterpart based on localVersion.
     * LocalVersion comes from metadata file stored in .mergin folder.
     * Note: this is just for scenarios where you only have LocalProject instance and not Project,
     * \note Project->isMergin() is recommended to use over this one
     */
    bool hasMerginMetadata() const { return localVersion > -1; }

    bool operator ==( const LocalProject &other ) const
    {
      return ( this->id() == other.id() );
    }

    bool operator !=( const LocalProject &other ) const
    {
      return !( *this == other );
    }
};

/**
 * \brief The MerginProject struct is used for projects that comes from Mergin.
 * This struct is used in the \see Projects struct and also for communication between MerginAPI and ProjectsModel
 */
struct MerginProject
{
    MerginProject() = default;
    ~MerginProject() = default;

    QString projectName;
    QString projectNamespace;

    /**
     * Returns the project ID or empty string if no ID is known. Then it's necessary to fetch the ID from API.
     */
    QString id() const;
    QString fullName() const;

    QDateTime serverUpdated; // available latest version of project files on server
    int serverVersion = -1;

    ProjectStatus::Status status = ProjectStatus::NoVersion;

    QString remoteError; // Error leading to project not being able to sync (received error code from server)

    bool isValid() const { return !projectName.isEmpty() && !projectNamespace.isEmpty(); }

    bool operator ==( const MerginProject &other ) const
    {
      return ( this->id() == other.id() );
    }

    bool operator !=( const MerginProject &other ) const
    {
      return !( *this == other );
    }

  private:
    QString projectId;
};

/**
 * \brief The Project struct serves as a struct for any kind of project (local/mergin).
 * It consists of two main parts - mergin and local.
 * Both parts are pointers to their specific structs and based on the pointer value (nullptr or assigned) these structs
 * decide if the project is local, mergin or both.
 */
struct Project
{
  Project() = default;
  ~Project() = default;

  MerginProject mergin;
  LocalProject local;

  bool isMergin() const { return mergin.isValid(); }
  bool isLocal() const { return local.isValid(); }

  QString projectName() const
  {
    if ( isMergin() ) return mergin.projectName;
    if ( isLocal() ) return local.projectName;
    return {};
  }

  QString projectNamespace() const
  {
    if ( isMergin() ) return mergin.projectNamespace;
    if ( isLocal() ) return local.projectNamespace;
    return {};
  }

  QString id() const
  {
    if ( isMergin() ) return mergin.id();
    if ( isLocal() ) return local.id();
    return {};
  }

  QString fullName() const
  {
    if ( isMergin() ) return mergin.fullName();
    if ( isLocal() ) return local.fullName();
    return {};
  }

  bool operator ==( const Project &other ) const
  {
    if ( this->isLocal() && other.isLocal() )
    {
      return this->local.id() == other.local.id() && this->local.fullName() == other.local.fullName();
    }
    if ( this->isMergin() && other.isMergin() )
    {
      return this->mergin.id() == other.mergin.id() && this->mergin.fullName() == other.mergin.fullName();
    }
    return false;
  }

  bool operator !=( const Project &other ) const
  {
    return !( *this == other );
  }
};

typedef QList<MerginProject> MerginProjectsList;
typedef QHash<QString, LocalProject> LocalProjectsDict;
Q_DECLARE_METATYPE( MerginProjectsList )

#endif // PROJECT_H
