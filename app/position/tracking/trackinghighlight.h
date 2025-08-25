/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef POSITIONTRACKINGHIGHLIGHT_H
#define POSITIONTRACKINGHIGHLIGHT_H

#include <QObject>
#include <qglobal.h>

#include "qgsgeometry.h"

class InputMapSettings;

class TrackingHighlight : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QgsPoint mapPosition READ mapPosition WRITE setMapPosition NOTIFY mapPositionChanged )
    Q_PROPERTY( InputMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )

    // Geometry in
    Q_PROPERTY( QgsGeometry trackedGeometry READ trackedGeometry WRITE setTrackedGeometry NOTIFY trackedGeometryChanged )

    // Geometry out
    Q_PROPERTY( QgsGeometry highlightGeometry READ highlightGeometry NOTIFY highlightGeometryChanged )

  public:
    explicit TrackingHighlight( QObject *parent = nullptr );

    QgsGeometry trackedGeometry() const;
    void setTrackedGeometry( const QgsGeometry &newTrackedGeometry );

    QgsGeometry highlightGeometry() const;

    QgsPoint mapPosition() const;
    void setMapPosition( QgsPoint newMapPosition );

    InputMapSettings *mapSettings() const;
    void setMapSettings( InputMapSettings *newMapSettings );

  public slots:
    void onPositionChanged();
    void onTrackedGeometryChanged();

  signals:
    void trackedGeometryChanged( QgsGeometry trackedGeometry );
    void highlightGeometryChanged( QgsGeometry highlightGeometry );

    void mapPositionChanged( QgsPoint mapPosition );
    void mapSettingsChanged();

  private:
    void setHighlightGeometry( const QgsGeometry &newHighlightGeometry );

    QgsPoint mMapPosition;
    InputMapSettings* mMapSettings = nullptr; // not owned

    QgsGeometry mTrackedGeometry;
    QgsGeometry mHighlightGeometry;
};

#endif // POSITIONTRACKINGHIGHLIGHT_H
