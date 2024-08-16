#ifndef MEASUREMENTMAPTOOL_H
#define MEASUREMENTMAPTOOL_H

#include "abstractmaptool.h"

#include <QObject>
#include <qglobal.h>
#include "qgspolygon.h"
#include "qgsvectorlayerutils.h"
#include "qgsmultipoint.h"
#include "qgsmultilinestring.h"
#include "qgspolygon.h"
#include "qgsmultipolygon.h"
#include "qgsrendercontext.h"
#include "qgsvectorlayereditbuffer.h"

#include "qgsvectorlayer.h"

class MeasurementMapTool : public AbstractMapTool
{
    Q_OBJECT
public:
    explicit MeasurementMapTool( QObject *parent = nullptr );
    virtual ~MeasurementMapTool() override;

//     Q_INVOKABLE void addPoint( const QgsPoint &point );

//     Q_INVOKABLE double totalDistance() const;

//     Q_INVOKABLE double calculatePerimeter() const;

//     Q_INVOKABLE double calculateArea() const;

//     Q_INVOKABLE void clearMeasurements();

//     Q_INVOKABLE QList<QgsPointXY> points() const;

//     Q_INVOKABLE bool isPolygon() const;


// signals:
//     void distanceMeasured(double distance);
//     void polygonCanBeFormed(bool canForm);

// private:
//     QList<QgsPointXY> mPoints;
//     QList<double> mDistances;
//     bool mIsPolygon;
};

#endif // MEASUREMENTMAPTOOL_H
