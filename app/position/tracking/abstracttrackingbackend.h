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

class AbstractTrackingBackend : public QObject
{
    Q_OBJECT

    Q_PROPERTY( UpdateFrequency updateFrequency READ updateFrequency WRITE setUpdateFrequency NOTIFY updateFrequencyChanged )

  public:
    enum UpdateFrequency // TODO: here we want to drop the time-based aspect and only use the distance-based approach (like in iOS)
    {
      Often = 0,
      Normal,
      Occasional,
    };
    Q_ENUM( UpdateFrequency );

    explicit AbstractTrackingBackend(
      UpdateFrequency updateFrequency = Often,
      QObject *parent = nullptr
    );

    UpdateFrequency updateFrequency() const;
    void setUpdateFrequency( const UpdateFrequency &newUpdateFrequency );

  signals:
    void positionUpdated();

    void updateFrequencyChanged( AbstractTrackingBackend::UpdateFrequency updateFrequency );

  protected:

    //! Stores the point to the tracking file and notifies tracking manager in case the app is in foreground
    void storeDataAndNotify( double x, double y, double z, double m );

  private:
    QFile mFile; // File for storing position updates

    UpdateFrequency mUpdateFrequency;
};

#endif // ABSTRACTTRACKINGBACKEND_H
