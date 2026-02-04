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
 *  have different set-ups. That's why it should be reloaded on project open.
 *
 *  \todo This class could have a nicer API. With just one public method accepting GeoPosition and provider specifier.
 */
class PositionTransformer : QObject
{
    Q_OBJECT
  public:
    PositionTransformer( const QgsCoordinateReferenceSystem &sourceCrs, const QgsCoordinateReferenceSystem &destinationCrs, bool verticalPassThroughEnabled, QObject *parent = nullptr );

    GeoPosition processBluetoothPosition( GeoPosition geoPosition );
    GeoPosition processAndroidPosition( GeoPosition geoPosition );
    GeoPosition processInternalAndroidPosition( const QGeoPositionInfo &geoPosition );
    GeoPosition processInternalIosPosition( QGeoPositionInfo &geoPosition );
    GeoPosition processInternalDesktopPosition( const QGeoPositionInfo &geoPosition );
    GeoPosition processSimulatedPosition( const GeoPosition &geoPosition );

  private:
    QgsCoordinateReferenceSystem mSourceCrs;
    QgsCoordinateReferenceSystem mDestinationCrs;
    QgsCoordinateTransformContext mTransformContext;
    bool mVerticalPassThroughEnabled;
};


#endif //POSITIONTRANSFORMER_H