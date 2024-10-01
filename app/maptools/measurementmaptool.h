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

#include "abstractmaptool.h"
#include <qglobal.h>
#include "qgsdistancearea.h"
#include "qgsgeometry.h"
#include "inpututils.h"
#include "qgspolygon.h"
#include "qgsgeometry.h"
#include "qgsvectorlayer.h"
#include "qgsmultipoint.h"

const double CLOSE_THRESHOLD = 10.0; // in pixels

class MeasurementMapTool : public AbstractMapTool
{
    Q_OBJECT

    Q_PROPERTY( QgsGeometry recordedGeometry READ recordedGeometry WRITE setRecordedGeometry NOTIFY recordedGeometryChanged )
    Q_PROPERTY( QgsGeometry existingVertices READ existingVertices WRITE setExistingVertices NOTIFY existingVerticesChanged )
    Q_PROPERTY( QPointF crosshairPoint READ crosshairPoint WRITE setCrosshairPoint NOTIFY crosshairPointChanged )

    Q_PROPERTY( double lengthWithGuideline READ lengthWithGuideline WRITE setLengthWithGuideline NOTIFY lengthWithGuidelineChanged )
    Q_PROPERTY( double perimeter READ perimeter WRITE setPerimeter NOTIFY perimeterChanged )
    Q_PROPERTY( double area READ area WRITE setArea NOTIFY areaChanged )

    Q_PROPERTY( bool canUndo READ canUndo WRITE setCanUndo NOTIFY canUndoChanged )
    Q_PROPERTY( bool canCloseShape READ canCloseShape WRITE setCanCloseShape NOTIFY canCloseShapeChanged )
    Q_PROPERTY( bool canFinalizeMeasurement READ canFinalizeMeasurement WRITE setCanFinalizeMeasurement NOTIFY canFinalizeMeasurementChanged )
    Q_PROPERTY( bool measurementFinalized READ measurementFinalized WRITE setMeasurementFinalized NOTIFY measurementFinalizedChanged )

  public:
    explicit MeasurementMapTool( QObject *parent = nullptr );
    virtual ~MeasurementMapTool() override;

    /**
     * Adds point to the end of the recorded geometry; updates recordedGeometry afterwards
     * Passed point needs to be in map CRS
     */
    Q_INVOKABLE void addPoint();

    /**
     *  Removes last point from recorded geometry if there is at least one point
     *  Updates recordedGeometry afterwards
     */
    Q_INVOKABLE void removePoint();

    /**
     * If there are at least 3 points, forms a polygon from recorded points so far.
     * Updates recorded geometry and calculates area and perimeter of formed polygon.
    */
    Q_INVOKABLE void finalizeMeasurement( bool closeShapeClicked );

    /**
     * Repeats measurement process.
     * Clears all recorded points and rebuilds the geometry.
     */
    Q_INVOKABLE void resetMeasurement();

    // Getter and Setters
    double lengthWithGuideline() const;
    void setLengthWithGuideline( const double &length );

    double perimeter() const;
    void setPerimeter( const double &perimeter );

    double area() const;
    void setArea( const double &area );

    QPointF crosshairPoint() const;
    void setCrosshairPoint( const QPointF &point );

    bool canUndo() const;
    void setCanUndo( bool newCanUndo );

    bool canCloseShape() const;
    void setCanCloseShape( bool newCanCloseShape );

    bool canFinalizeMeasurement() const;
    void setCanFinalizeMeasurement( bool canFinalize );

    bool measurementFinalized() const;
    void setMeasurementFinalized( bool newMeasurementFinalized );

    const QgsGeometry &recordedGeometry() const;
    void setRecordedGeometry( const QgsGeometry &newRecordedGeometry );

    QgsGeometry existingVertices() const;
    void setExistingVertices( const QgsGeometry &vertices );

    void setMapSettings( InputMapSettings *newMapSettings ) override;

  signals:
    void lengthWithGuidelineChanged( const double &lengthWithGuideline );
    void perimeterChanged( const double &perimeter );
    void areaChanged( const double &area );
    void canUndoChanged( bool canUndo );
    void canCloseShapeChanged( bool canUndo );
    void measurementFinalizedChanged( bool measurementFinalized );
    void recordedGeometryChanged( const QgsGeometry &recordedGeometry );
    void existingVerticesChanged( const QgsGeometry &vertices );
    void crosshairPointChanged( const QPointF &crosshairPoint );
    void canFinalizeMeasurementChanged( bool canFinalize );

  protected:
    void rebuildGeometry();
    void checkCanCloseShape();

  public slots:
    void updateDistance();

  private:
    QVector<QgsPoint> mPoints;
    QgsGeometry mRecordedGeometry;
    QgsGeometry mExistingVertices;
    QgsDistanceArea mDistanceArea;
    QPointF mCrosshairPoint;
    double mLengthWithGuideline = 0;
    double mPerimeter = 0;
    double mArea = 0;
    bool mCanUndo = false;
    bool mCanCloseShape = false;
    bool mCanFinalizeMeasurement = false;
    bool mMeasurementFinalized = false;
};

#endif // MEASUREMENTMAPTOOL_H
