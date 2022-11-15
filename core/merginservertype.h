/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MERGINSERVERTYPE_H
#define MERGINSERVERTYPE_H

#include <QObject>

class MerginServerType
{
    Q_GADGET
  public:
    explicit MerginServerType();

    enum ServerType
    {
      OLD, // old, no workspaces
      CE, // ce, single global workspace
      EE, // ee, multiple workspaces
      SAAS // saas, multiple workspaces and subscriptions
    };
    Q_ENUMS( ServerType )
};

#endif // MERGINSERVERTYPE_H
