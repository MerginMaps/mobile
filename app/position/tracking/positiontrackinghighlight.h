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

class PositionTrackingHighlight : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QgsPoint mapPosition READ mapPosition WRITE setMapPosition NOTIFY mapPositionChanged )

    // Geometry in
    Q_PROPERTY( QgsGeometry trackedGeometry READ trackedGeometry WRITE setTrackedGeometry NOTIFY trackedGeometryChanged )

    // Geometry out
    Q_PROPERTY( QgsGeometry highlightGeometry READ highlightGeometry NOTIFY highlightGeometryChanged )

  public:
    explicit PositionTrackingHighlight( QObject *parent = nullptr );

    QgsGeometry trackedGeometry() const;
    void setTrackedGeometry( const QgsGeometry &newTrackedGeometry );

    QgsGeometry highlightGeometry() const;

    QgsPoint mapPosition() const;
    void setMapPosition( QgsPoint newMapPosition );

  public slots:
    void recalculate();

  signals:
    void trackedGeometryChanged( QgsGeometry trackedGeometry );
    void highlightGeometryChanged( QgsGeometry highlightGeometry );

    void mapPositionChanged( QgsPoint mapPosition );

  private:
    void setHighlightGeometry( const QgsGeometry &newHighlightGeometry );

    QgsGeometry mTrackedGeometry;
    QgsGeometry mHighlightGeometry;
    QgsPoint mMapPosition;
};

#endif // POSITIONTRACKINGHIGHLIGHT_H
