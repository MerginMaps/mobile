/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MERGINAPISTATUS_H
#define MERGINAPISTATUS_H

#include <QObject>

class MerginApiStatus
{
    Q_GADGET
  public:
    explicit MerginApiStatus();

    enum VersionStatus
    {
      UNKNOWN, // unchecked
      PENDING, // sent version check, but waiting for the response
      NOT_FOUND, // cannot be checked due to network/wrong url
      OK, // server version satisfied requirements
      INCOMPATIBLE // server version below requirements
    };
    Q_ENUMS( VersionStatus )
};

#endif // MERGINAPISTATUS_H
