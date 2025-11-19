/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef POSITIONDIRECTION_H
#define POSITIONDIRECTION_H

#include <QObject>
#include <QTimer>
#include <QOrientationSensor>
#include <QCompass>

#include "positionkit.h"
#include "compass.h"


/**
 * Utility class containing information about the direction. Note that depends on availibility of sensors and their data.
 *
 * Updates direction periodicly according timer while filtering small difference values between old and new direction angle.
 * Uses ground speed (in m/s) to select which direction source will be used - compass for smaller speed whereas positionKit direction
 * for speed over speedLimit.
 */
class PositionDirection : public QObject
{
    Q_OBJECT

    Q_PROPERTY( qreal direction READ direction NOTIFY directionChanged )
    Q_PROPERTY( bool hasDirection READ hasDirection NOTIFY hasDirectionChanged )

    // components to use during calculation
    Q_PROPERTY( Compass *compass READ compass WRITE setCompass NOTIFY compassChanged )
    Q_PROPERTY( PositionKit *positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged )

  public:
    explicit PositionDirection( QObject *parent = nullptr );

    qreal direction() const;

    PositionKit *positionKit() const;
    void setPositionKit( PositionKit *positionKit );

    bool hasDirection() const;
    void setHasDirection( bool hasDirection );

    Compass *compass() const;
    void setCompass( Compass *compass );

  signals:
    void directionChanged();
    void positionKitChanged();
    void compassChanged();
    void hasDirectionChanged();

  public slots:
    void updateDirection();

  private:
    qreal mDirection = Compass::MIN_INVALID_DIRECTION;
    bool mHasDirection = false;
    PositionKit *mPositionKit = nullptr;
    Compass *mCompass = nullptr;
    QTimer mTimer;
    const qreal mUpdateMinAngleDelta = 3; //!< in degrees.
    const qreal mSpeedLimit = 4.16;  //!< 4.16 m/s ~= 15km/h. Over speed limit, directions depends on direction of movement.
    //! Returns difference of angles. Result is in interval <0,180> degrees.
    qreal angleBetween( qreal d1, qreal d2 );
};

#endif // POSITIONDIRECTION_H
