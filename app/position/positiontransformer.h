/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef POSITIONTRANSFORMER_H
#define POSITIONTRANSFORMER_H

#include <QGeoPositionInfo>

#include <qgscoordinatereferencesystem.h>
#include <qgscoordinatetransformcontext.h>

#include "geoposition.h"

/**
 *  PositionTransformer is a utility class, which should be used with providers to transform received position into set
 *  CRS. Currently only elevation is transformed! The PositionTransformer is project specific as different projects can
 *  have different set-ups.
 *
 *  \todo This class could have a nicer API. With just one public method accepting GeoPosition and provider specifier.
 */
class PositionTransformer : QObject
{
    Q_OBJECT
  public:
    PositionTransformer( const QgsCoordinateReferenceSystem &sourceCrs, const QgsCoordinateReferenceSystem &destinationCrs, bool verticalPassThroughEnabled, QObject *parent = nullptr );

    /**
    * Transform the elevation from EPSG:4979 (WGS84 (EPSG:4326) + ellipsoidal height) to specified geoid model
    * (by default EPSG:9707 (WGS84 + EGM96))
    * Transform only if:
    *  1. the position is not mocked, and it's ellipsoidal elevation
    *  2. the position is mocked, the elevation is ellipsoidal plus pass through is disabled
    * \note This method should be used only with AndroidPositionProvider, which guarantees the elevation to be ellipsoid
    * \return Copy of passed geoPosition with processed elevation and elevation separation if applicable.
    */
    GeoPosition processAndroidPosition( GeoPosition geoPosition );

    /**
     * Transform the elevation if the user sets custom vertical CRS. The elevation gets recalculated to ellipsoid elevation
     * and then back to orthometric based on specified CRS.
     * \note This method should be used only with Bluetooth PositionProvider to mitigate unnecessary transformations
     * \return Copy of passed geoPosition with processed elevation and elevation separation.
     */
    GeoPosition processBluetoothPosition( GeoPosition geoPosition );

    /**
    * Transform the elevation from EPSG:4979 (WGS84 (EPSG:4326) + ellipsoidal height) to specified geoid model
    * (by default EPSG:9707 (WGS84 + EGM96))
    * \note This method should be used only with InternalPositionProvider on Android, which supplies QGeoPositionInfo
    * \return New GeoPosition with processed elevation and elevation separation if applicable.
    */
    GeoPosition processInternalAndroidPosition( const QGeoPositionInfo &geoPosition );

    /**
     * This doesn't transform the received elevation just passes it along. On Windows the elevation should be already
     * orthometric, however the elevation separation is unknown, thus we can't process it further.
     * \return New GeoPosition with received elevation.
     */
    GeoPosition processInternalDesktopPosition( const QGeoPositionInfo &geoPosition );

    /**
      * Transform the elevation from EPSG:4979 (WGS84 (EPSG:4326) + ellipsoidal height) to specified geoid model
      * (by default EPSG:9707 (WGS84 + EGM96))
      * Transform only if:
      *  1. the position is not mocked, and it's ellipsoidal elevation
      *  2. the position is mocked, the elevation is ellipsoidal plus pass through is disabled
      *  \note The function gets passed reference to geoPosition, because we use some QGeoPositionInfo attributes
      *       to find out if the position is mocked and if the elevation is ellipsoid or orthometric.
     * \return New GeoPosition with processed elevation and elevation separation if applicable.
     */
    GeoPosition processInternalIosPosition( QGeoPositionInfo &geoPosition );
    GeoPosition processSimulatedPosition( const GeoPosition &geoPosition );

  private:
    QgsCoordinateReferenceSystem mSourceCrs;
    QgsCoordinateReferenceSystem mDestinationCrs;
    QgsCoordinateTransformContext mTransformContext;
    bool mVerticalPassThroughEnabled;
};


#endif //POSITIONTRANSFORMER_H