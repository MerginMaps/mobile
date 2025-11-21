/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TRACKINGUTILS_H
#define TRACKINGUTILS_H

#include "qgscoordinatereferencesystem.h"
#include "qgsgeometry.h"

class QgsProject;
class QgsVectorLayer;

class VariablesManager;
class TrackingManager;

class TrackingUtils
{
    Q_GADGET

  public:

    enum UpdateFrequency // TODO: update values according to real values in backends!
    {
      BestAccuracy = 0,
      LessAccuracy,
      EvenWorseAccuracy,
    };
    Q_ENUM( UpdateFrequency );

    explicit TrackingUtils( ) = default;
    ~TrackingUtils() = default;

    //! Returns CRS used in position tracking
    static QgsCoordinateReferenceSystem crs();

    //! Returns if project admin enabled tracking option for this project
    static bool projectHasTrackingCapability( QgsProject *project );

    static QgsVectorLayer* getTrackingLayer( QgsProject *project );

    static UpdateFrequency getTrackingUpdateFrequency( QgsProject *project );

    static void storeTrack( QgsVectorLayer *targetLayer, TrackingManager *trackingManager, VariablesManager *variablesManager );

    /**
     * Parses position updates in format "<x> <y> <z> <m>\n<x>..."
     * and appends them to the provided linestring
     */
    static void parseAndAppendPositionUpdates( const QString &data, QgsGeometry &line );
};

#endif // TRACKINGUTILS_H
