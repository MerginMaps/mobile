/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "synchronizationcontroller.h"

#include "qdebug.h"

SynchronizationController::SynchronizationController( QObject *parent ) : QObject( parent )
{

}

void SynchronizationController::syncProject( const QString &projectId )
{
  std::shared_ptr<Project> project;

  if ( project == nullptr || !project->isMergin() || project->mergin->pending )
  {
    return;
  }


  if ( project->mergin->status == ProjectStatus::NoVersion || project->mergin->status == ProjectStatus::OutOfDate )
  {
//    bool useAuth = !mBackend->userAuth()->hasAuthData() && mModelType == ProjectModelTypes::PublicProjectsModel;
//    mBackend->updateProject( project->mergin->projectNamespace, project->mergin->projectName, useAuth );
  }
  else if ( project->mergin->status == ProjectStatus::Modified )
  {
//    mBackend->uploadProject( project->mergin->projectNamespace, project->mergin->projectName );
  }
}

void SynchronizationController::activeProjectChanged( LocalProject activeProject )
{
  qDebug() << "Active project changed to:" << activeProject.projectName;
}
