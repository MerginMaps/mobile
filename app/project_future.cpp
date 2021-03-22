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

#include <QDirIterator>

QString MerginProject_future::id() const
{
  return MerginApi::getFullProjectName( projectNamespace, projectName );
}

QString LocalProject_future::id() const
{
  if ( !projectName.isEmpty() && !projectNamespace.isEmpty() )
    return MerginApi::getFullProjectName( projectNamespace, projectName );

  QDir dir( projectDir );
  return dir.dirName();
}
