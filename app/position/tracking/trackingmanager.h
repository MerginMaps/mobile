/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TRACKINGMANAGER_H
#define TRACKINGMANAGER_H

#include <QObject>
#include <QTimer>
#include <QFile>

#include "trackingutils.h"
#include "abstracttrackingbackend.h"

#include "qgsgeometry.h"

class PositionKit;

class TrackingManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool isRunning READ isRunning NOTIFY isRunningChanged )
    Q_PROPERTY( QgsGeometry geometry READ geometry NOTIFY geometryChanged )

    Q_PROPERTY( QDateTime startTime READ startTime NOTIFY startTimeChanged )
    Q_PROPERTY( QString elapsedTimeText READ elapsedTimeText NOTIFY elapsedTimeTextChanged )

  public:

    explicit TrackingManager( QObject *parent = nullptr );

    //! Builds tracking backend and starts to listen to position updates.
    //! Position updates are stored to the tracking file in dataPath folder
    bool startTracking( QString dataPath, TrackingUtils::UpdateFrequency updateFrequency, PositionKit *positionKit );

    bool stopTracking();

    void reset();

    bool isRunning() const;

    //! Returns the current tracked geometry
    QgsGeometry geometry() const;

    QDateTime startTime() const;

    //! How long we have been tracking, formatted as a text (empty if not tracking)
    QString elapsedTimeText() const;

  public slots:

    void onPositionUpdated(); // handles position update from a backend

  signals:

    void isRunningChanged();
    void geometryChanged();

    void startTimeChanged();
    void elapsedTimeTextChanged();

  private:

    void buildBackend( TrackingUtils::UpdateFrequency updateFrequency, PositionKit *positionKit );

    bool mIsRunning = false;

    QFile mFile; // Tracking file that stores position updates from backends
    qint64 mFileOffset; // Remembers the last read position from the tracking file
    QReadWriteLock mFileLock; // Locks the tracking file for read/write as backends might work in a different thread

    QgsGeometry mGeometry; // Constructed geometry from positions within the tracking file, in GNSS CRS (WGS84)

    QDateTime mStartTime;
    QTimer mElapsedTimeTextTimer; // timer to make sure we are periodically updating tracking elapsed time

    std::unique_ptr<AbstractTrackingBackend> mBackend; // owned
};

#endif // TRACKINGMANAGER_H
