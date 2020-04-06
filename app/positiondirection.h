#ifndef POSITIONDIRECTION_H
#define POSITIONDIRECTION_H

#include <QObject>
#include <QTimer>

#ifdef MOBILE_OS
#include <QOrientationSensor>
#include <QCompass>
#endif

#include "qgsquickpositionkit.h"


/**
 * Utility class containing infortmation about the direction. Note that depends on availibility of sensors and their data.
 *
 * Updates direction periodicly according timer while filtering small difference values between old and new direction angle.
 * Uses ground speed (in m/s) to select which direction source will be used - compass for smaller speed whereas positionKit direction
 * for speed over speedLimit.
 */
class PositionDirection : public QObject
{
    Q_OBJECT

    Q_PROPERTY( qreal direction READ direction NOTIFY directionChanged )
    Q_PROPERTY( QgsQuickPositionKit *positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged )
    Q_PROPERTY( bool hasDirection READ hasDirection NOTIFY hasDirectionChanged )

  public:
    explicit PositionDirection( QObject *parent = nullptr );

    qreal direction() const;

    QgsQuickPositionKit *positionKit() const;
    void setPositionKit( QgsQuickPositionKit *positionKit );

    qreal minInvalidDirection() const;

    bool hasDirection() const;
    void setHasDirection( bool hasDirection );

  signals:
    void directionChanged();
    void positionKitChanged();
    void minInvalidDirectionChanged();
    void hasDirectionChanged();
  public slots:
#ifdef MOBILE_OS
    void updateDirection();
    void setUserOrientation();
#endif

  private:
    //! any direction value in degrees that is < -180 is not valid.
    qreal MIN_INVALID_DIRECTION = -180.1;
    qreal mDirection = MIN_INVALID_DIRECTION;
    bool mHasDirection = false;
    QgsQuickPositionKit *mPositionKit = nullptr;
#ifdef MOBILE_OS
    QOrientationSensor *mOrientationSensor = nullptr;
    QCompass *mCompass = nullptr;
    QTimer mTimer;
    const qreal mUpdateMinAngleDelta = 3; //! in degrees.
    const qreal mSpeedLimit = 4.16;  //! 4.16 m/s ~= 15km/h. Over speed limit, directions depends on direction of movement.
    //! Returns difference of angles. Result is in interval <0,180> degrees.
    qreal angleBetween( qreal d1, qreal d2 );
#endif
};

#endif // POSITIONDIRECTION_H
