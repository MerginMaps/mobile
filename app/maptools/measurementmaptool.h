/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MEASUREMENTMAPTOOL_H
#define MEASUREMENTMAPTOOL_H

#include "inputconfig.h"
#include "abstractmaptool.h"
#include <qglobal.h>
#include "qgsdistancearea.h"
#include "qgsgeometry.h"
#include <QgsPolygon.h>
#include <QgsLineString.h>
#include <QgsGeometry.h>

const double CLOSE_THRESHOLD = 100.0;

class MeasurementMapTool : public AbstractMapTool
{
    Q_OBJECT

    Q_PROPERTY( QgsGeometry recordedGeometry READ recordedGeometry WRITE setRecordedGeometry NOTIFY recordedGeometryChanged )

  public:
    explicit MeasurementMapTool( QObject *parent = nullptr );
    virtual ~MeasurementMapTool() override;

    /**
     * Adds point to the end of the recorded geometry; updates recordedGeometry afterwards
     * Passed point needs to be in active vector layer CRS
     */
    Q_INVOKABLE void addPoint( const QgsPoint &point );

    /**
     *  Removes last point from recorded geometry if there is at least one point
     *  Updates recordedGeometry afterwards
     */
    Q_INVOKABLE void removePoint();

    Q_INVOKABLE void closeShape();
    Q_INVOKABLE void  repeat();

    const QgsGeometry &recordedGeometry() const;
    void setRecordedGeometry( const QgsGeometry &newRecordedGeometry );

  signals:
    void recordedGeometryChanged( const QgsGeometry &recordedGeometry );

    void canCloseShape( bool canClose );
    void canUndo( bool canUndo );

    void shapeArea( double area );
    void shapePerimeter( double perimeter );

  protected:
    void rebuildGeometry();

  public slots:
    double updateDistance( const QgsPoint &crosshairPoint );

  private:
    QVector<QgsPoint> mPoints;
    QgsGeometry mRecordedGeometry;
};

#endif // MEASUREMENTMAPTOOL_H
