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

#include <QObject>
#include <QTimer>
#include <QOrientationSensor>
#include <QCompass>

class Compass: public QObject
{
    Q_OBJECT
  public:
    explicit Compass( QObject *parent = nullptr );

    //! any direction value in degrees that is < -180 is not valid.
    static constexpr qreal MIN_INVALID_DIRECTION = -180.1;

    qreal direction() const;
    QCompassReading *reading();

  signals:
    void directionChanged();
  public slots:
    void setUserOrientation();
  private:
    QOrientationSensor *mOrientationSensor = nullptr;
    QCompass *mCompass = nullptr;
};

#endif // COMPASS_H
