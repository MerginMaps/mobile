/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MAPPOSITION_H
#define MAPPOSITION_H

#include <QObject>
#include <qglobal.h>

#include "position/positionkit.h"
#include "qgsquickmapsettings.h"

/**
 * MapPosition utility class converts real world position into map related attributes
 * in order to show position marker, accuracy circle and more.
 */
class MapPosition : public QObject
{
    Q_OBJECT

    // GPS position in map coords
    Q_PROPERTY( QgsPoint mapPosition READ mapPosition NOTIFY mapPositionChanged )

    // GPS position in device coords (pixels)
    Q_PROPERTY( QgsPointXY screenPosition READ screenPosition NOTIFY screenPositionChanged )

    // Screen horizontal accuracy, 2 if not available or resolution is too small
    Q_PROPERTY( double screenAccuracy READ screenAccuracy NOTIFY screenAccuracyChanged )

    // Components to use during calculation
    Q_PROPERTY( PositionKit *positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged )
    Q_PROPERTY( QgsQuickMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )

  public:
    explicit MapPosition( QObject *parent = nullptr );

    PositionKit *positionKit() const;
    void setPositionKit( PositionKit *newPositionKit );

    QgsQuickMapSettings *mapSettings() const;
    void setMapSettings( QgsQuickMapSettings *newMapSettings );

    QgsPoint mapPosition() const;
    QgsPointXY screenPosition() const;
    double screenAccuracy() const;

  signals:
    void positionKitChanged( PositionKit * );
    void mapSettingsChanged( QgsQuickMapSettings * );

    void mapPositionChanged( QgsPoint );
    void screenPositionChanged( QgsPointXY );
    void screenAccuracyChanged( double );

  public slots:
    void update();

  private:

    void recalculateMapPosition();
    void recalculateScreenPosition();
    void recalculateScreenAccuracy();

    QgsPoint mMapPosition;
    QgsPointXY mScreenPosition;
    double mScreenAccuracy = 2;

    PositionKit *mPositionKit = nullptr; // not owned
    QgsQuickMapSettings *mMapSettings = nullptr; // not owned
};

#endif // MAPPOSITION_H
