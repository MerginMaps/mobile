/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "project_future.h"

#include "merginapi.h"

void LocalProject_future::copyValues( const LocalProject_future &other )
{
  projectName = other.projectName;
  projectNamespace = other.projectNamespace;
  projectDir = other.projectDir;
  projectError = other.projectError;
  qgisProjectFilePath = other.qgisProjectFilePath;
  localVersion = other.localVersion;
}

QString MerginProject_future::id()
{
  return MerginApi::getFullProjectName( projectNamespace, projectName );
}

QString LocalProject_future::id()
{
  if ( !projectName.isEmpty() && !projectNamespace.isEmpty() )
    return MerginApi::getFullProjectName( projectNamespace, projectName );

  return projectDir;
}
