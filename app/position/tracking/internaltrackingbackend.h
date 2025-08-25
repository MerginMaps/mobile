/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INTERNALTRACKINGBACKEND_H
#define INTERNALTRACKINGBACKEND_H

#include "abstracttrackingbackend.h"
#include <QDateTime>

class PositionKit;

class InternalTrackingBackend : public AbstractTrackingBackend
{
    Q_OBJECT

  public:

    explicit InternalTrackingBackend(
      QReadWriteLock *fileLock,
      PositionKit *positionKit,
      TrackingUtils::UpdateFrequency updateFrequency,
      QObject *parent = nullptr
    );

  private:

    QDateTime mLastUpdate;
    double mUpdateInterval = 0; // ms

    PositionKit *mPositionKit = nullptr; // not owned
};

#endif // INTERNALTRACKINGBACKEND_H
