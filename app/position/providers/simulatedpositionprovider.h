/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SIMULATEDPOSITIONPROVIDER_H
#define SIMULATEDPOSITIONPROVIDER_H

#include "abstractpositionprovider.h"

#include <QObject>
#include <QTimer>

#include <random>

/**
 * SimulatedPositionProvider is used to generate random position around specified point
 * Point can be specified via constructor arguments
 * Should be used as a position provider in PositionKit
 */
class SimulatedPositionProvider : public AbstractPositionProvider
{
    Q_OBJECT

  public:

    /**
     *  Change these default values to fly around a specific point
     *  Set flightRadius to 0 in order to get constant position (no movement)
     */
    explicit SimulatedPositionProvider(
      double longitude = 17.107137342092614,
      double latitude = 48.10301740375036,
      double flightRadius = 0,
      double updateTimeout = 1000,
      QObject *parent = nullptr
    );
    ~SimulatedPositionProvider() override;

    void setUpdateInterval( double msecs ) override;

  public slots:
    void startUpdates() override;
    void stopUpdates() override;
    void closeProvider() override;

    void setPosition( QgsPoint position ) override;

    void generateNextPosition();

  private:
    void generateRadiusPosition();
    void generateConstantPosition();

    std::unique_ptr<QTimer> mTimer;
    std::unique_ptr<std::mt19937> mGenerator;
    double mAngle = 0;
    double mLongitude = 0;
    double mLatitude = 0;
    double mFlightRadius = 0;
    double mTimerTimeout = 0;
};

#endif // SIMULATEDPOSITIONPROVIDER_H
