/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROJECT_FUTURE_H
#define PROJECT_FUTURE_H

#include <QObject>
#include <qdebug.h>
#include <QDateTime>
#include <memory>

enum ProjectStatus_future
{
  _NoVersion,  //!< the project is not available locally
  _UpToDate,   //!< both server and local copy are in sync with no extra modifications
  _OutOfDate,  //!< server has newer version than what is available locally (but the project is not modified locally)
  _Modified,    //!< there are some local modifications in the project that need to be pushed (note: also server may have newer version)
  // TODO: add orphaned state
};
Q_ENUMS( ProjectStatus_future )

struct LocalProject_future
{
  LocalProject_future() {}; /*{ qDebug() << "Building LocalProject_future " << this; }*/
  ~LocalProject_future() {}; /*{ qDebug() << "Removing LocalProject_future " << this; }*/

  QString projectName;
  QString projectNamespace;

  QString id();

  QString projectDir;
  QString projectError; // Error that leads to project not being able to open in app

  QString qgisProjectFilePath;

  int localVersion = -1;

  void copyValues( const LocalProject_future &other );
};

struct MerginProject_future
{
  MerginProject_future() {}; /*{ qDebug() << "Building MerginProject_future " << this; }*/
  ~MerginProject_future() {}; /*{ qDebug() << "Removing MerginProject_future " << this; }*/

  QString projectName;
  QString projectNamespace;

  QString id();
  QDateTime serverUpdated; // available latest version of project files on server
  int serverVersion;

  ProjectStatus_future status = ProjectStatus_future::_NoVersion;
  bool pending = false;

  qreal progress = 0;

  // Maybe better use enum or int for error code
  QString remoteError; // Error leading to project not being able to sync
};

struct Project_future
{
  Project_future() {}; /*{ qDebug() << "Building Project_future " << this; }*/
  ~Project_future() {}; /*{ qDebug() << "Removing Project_future " << this; }*/

  std::unique_ptr<MerginProject_future> mergin;
  std::unique_ptr<LocalProject_future> local;

  bool isMergin() { return mergin != nullptr; }
  bool isLocal() { return local != nullptr; }
};

#endif // PROJECT_FUTURE_H
