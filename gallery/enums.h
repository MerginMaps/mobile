/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ENUMS_H
#define ENUMS_H

#include <QObject>

class StreamingIntervalType
{
    Q_GADGET
  public:
    explicit StreamingIntervalType() {}

    enum IntervalType
    {
      Time = 0, // time-based recording
      Distance, // distance-based recording
    };
    Q_ENUMS( IntervalType )
};

class PositionProviderType
{
    Q_GADGET
  public:
    explicit PositionProviderType() {}

    enum ProviderType
    {
      Connecting = 0,
      Connected,
      NoConnection,
      WaitingToReconnect
    };
    Q_ENUMS( ProviderType )
};

class ProjectStatus
{
    Q_GADGET
    public:
    explicit ProjectStatus() {}

    enum Status
    {
      NoVersion,  //!< the project is not downloaded
      UpToDate,   //!< both server and local copy are in sync with no extra modifications
      NeedsSync,  //!< server has newer version than what is available locally and/or the project is modified locally
    };
    Q_ENUMS( Status )
};

#endif // ENUMS_H
