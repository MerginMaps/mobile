#include "measurementmaptool.h"

#include <QObject>
#include <QList>
#include "qgspointxy.h"
#include "qgsgeometryutils.h"

MeasurementMapTool::MeasurementMapTool( QObject *parent )
  : AbstractMapTool{parent}
{


}

MeasurementMapTool::~MeasurementMapTool() = default;

// void MeasurementMapTool::addPoint( const QgsPointXY &point )
// {
//     if (mPoints.isEmpty()) {
//         mPoints.append(point);
//         return;
//     }

//     // Calculate distance from the last point to the new one
//     QgsPointXY lastPoint = mPoints.last();
//     double distance = QgsGeometryUtils::sqrDistance2D(lastPoint, point);
//     mDistances.append(distance);

//     mPoints.append(point);

//     emit distanceMeasured(distance);

//     // Check if a polygon can be formed
//     if (mPoints.size() > 2) {
//         mIsPolygon = true;
//         emit polygonCanBeFormed(true);
//     }
// }

// double MeasurementMapTool::totalDistance() const
// {
//     double total = 0.0;
//     for (const double &d : mDistances) {
//         total += d;
//     }
//     return total;
// }

// double MeasurementMapTool::calculatePerimeter() const
// {
//     if (mIsPolygon) {
//         double perimeter = totalDistance();
//         // Closing the polygon (last point to the first point)
//         perimeter += QgsGeometryUtils::sqrDistance2D(mPoints.last(), mPoints.first());
//         return perimeter;
//     }
//     return 0.0;
// }

// double MeasurementMapTool::calculateArea() const
// {
//     if (mIsPolygon) {
//         QgsPolygonXY polygon;
//         polygon.setExteriorRing(QgsLineString(mPoints));
//         return polygon.area();
//     }
//     return 0.0;
// }

// void MeasurementMapTool::clearMeasurements()
// {
//     mPoints.clear();
//     mDistances.clear();
//     mIsPolygon = false;
//     emit polygonCanBeFormed(false);
// }

// QList<QgsPointXY> MeasurementMapTool::points() const
// {
//     return mPoints;
// }

// bool MeasurementMapTool::isPolygon() const
// {
//     return mIsPolygon;
// }
