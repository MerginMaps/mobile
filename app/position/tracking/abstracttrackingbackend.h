/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABSTRACTTRACKINGBACKEND_H
#define ABSTRACTTRACKINGBACKEND_H

#include <QObject>
#include <QFile>

#include "trackingutils.h"

class AbstractTrackingBackend : public QObject
{
    Q_OBJECT

  public:

    explicit AbstractTrackingBackend(
      QReadWriteLock *fileLock,
      TrackingUtils::UpdateFrequency updateFrequency = TrackingUtils::BestAccuracy,
      QObject *parent = nullptr
    );

    virtual ~AbstractTrackingBackend();

  signals:
    void positionUpdated();

    void updateFrequencyChanged( TrackingUtils::UpdateFrequency updateFrequency );

  protected:

    //! Stores the point to the tracking file and notifies tracking manager in case the app is in foreground
    void storeDataAndNotify( double x, double y, double z, double m );

  private:
    QFile mFile; // File for storing position updates
    QReadWriteLock *mFileLock = nullptr; // not owned

    TrackingUtils::UpdateFrequency mUpdateFrequency;
};

#endif // ABSTRACTTRACKINGBACKEND_H
