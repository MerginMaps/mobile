/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef COMPASS_H
#define COMPASS_H

#include <QElapsedTimer>
#include <QObject>
#include <QOrientationSensor>
#include <QCompass>

class QRotationSensor;

class Compass: public QObject
{
    Q_OBJECT
  public:
    explicit Compass( QObject *parent = nullptr );

    //! any direction value in degrees that is < -180 is not valid.
    static constexpr qreal MIN_INVALID_DIRECTION = -180.1;

    qreal direction() const;

  signals:
    void directionChanged();
  public slots:
    void setUserOrientation();
  private slots:
    void onReadingChanged();
  private:

    /**
     * Returns the azimuth correction (in degrees) for the UI orientation relative to the native orientation.
     * \param deviceOrientation only tells to which side the UI is rotated
     */
    static int userOrientation( Qt::ScreenOrientation screenOrientation, Qt::ScreenOrientation nativeOrientation,
                                QOrientationReading::Orientation deviceOrientation, int currentUserOrientation );

    //! Adds an azimuth (in degrees) to the smoothed one, \a elapsed is the time since the previous one (in milliseconds)
    void addAzimuth( qreal azimuth, qint64 elapsed );

    //! Returns the smoothed azimuth in degrees, in range (-180, 180]
    qreal smoothedAzimuth() const;

    //! Converts QRotationReading::z (counter-clockwise, 0 at north) to an azimuth in degrees, in range (-180, 180]
    static qreal azimuthFromRotation( qreal rotationZ );

    QOrientationSensor *mOrientationSensor = nullptr;
    QCompass *mCompass = nullptr;

    // Android only: fused orientation (gyroscope, magnetometer, accelerometer), preferred to the noisy raw compass.
    // Null if the device does not have it.
    QRotationSensor *mRotationSensor = nullptr;
    bool mHasRotationReading = false;

    // smoothed azimuth as a vector, so it handles the wrap around +-180
    bool mSmoothing = false;
    qreal mSmoothedX = 0;
    qreal mSmoothedY = 0;
    bool mHasSmoothedAzimuth = false;
    QElapsedTimer mSmoothingTimer; // time since the last reading

    friend class TestUtilsFunctions;
};

#endif // COMPASS_H
