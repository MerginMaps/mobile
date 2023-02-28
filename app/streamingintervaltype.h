/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STREAMINGINTERVALTYPE_H
#define STREAMINGINTERVALTYPE_H

#include <QObject>

class StreamingIntervalType
{
    Q_GADGET
  public:
    explicit StreamingIntervalType();

    enum IntervalType
    {
      Time = 0, // time-based recording
      Distance, // distance-based recording
    };
    Q_ENUMS( IntervalType )
};

#endif // STREAMINGINTERVALTYPE_H
