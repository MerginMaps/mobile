/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SNAPUTILS_H
#define SNAPUTILS_H

#include <QObject>
#include <qglobal.h>

#include "qgspoint.h"
#include "qgsproject.h"
#include "qgscoordinatereferencesystem.h"
#include "qgsquickmapsettings.h"

#include "qgssnappingutils.h"

class SnapUtils : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QgsProject *qgsProject READ qgsProject WRITE setQgsProject NOTIFY qgsProjectChanged )
    Q_PROPERTY( QgsQuickMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )
    Q_PROPERTY( QPointF centerPosition READ centerPosition WRITE setCenterPosition NOTIFY centerPositionChanged )
    Q_PROPERTY( bool snapped READ snapped WRITE setSnapped NOTIFY snappedChanged )
    Q_PROPERTY( QgsPoint snappedPosition READ snappedPosition WRITE setSnappedPosition NOTIFY snappedPositionChanged )
    Q_PROPERTY( SnapType snapType READ snapType WRITE setSnapType NOTIFY snapTypeChanged )
    Q_PROPERTY( bool useSnapping READ useSnapping WRITE setUseSnapping NOTIFY useSnappingChanged )
    Q_PROPERTY( QgsVectorLayer *destinationLayer READ destinationLayer WRITE setDestinationLayer NOTIFY destinationLayerChanged )

  public:
    SnapUtils( QObject *parent = nullptr );

    enum SnapType
    {
      Vertex = 0,
      Segment,
      Other,
    };
    Q_ENUM( SnapType );

    QgsProject *qgsProject() const;
    void setQgsProject( QgsProject *newQgsProject );

    QgsQuickMapSettings *mapSettings() const;
    void setMapSettings( QgsQuickMapSettings *newMapSettings );

    Q_INVOKABLE void getsnap( QPointF mapPoint );

    QPointF centerPosition() const;
    void setCenterPosition( QPointF newCenterPosition );

    QgsPoint snappedPosition() const;
    void setSnappedPosition( QgsPoint newSnappedPosition );

    bool snapped() const;
    void setSnapped( bool newSnapped );

    bool useSnapping() const;
    void setUseSnapping( bool useSnapping );

    const SnapUtils::SnapType &snapType() const;
    void setSnapType( const SnapUtils::SnapType &newSnapType );

    QgsCoordinateReferenceSystem destinationCrs() const;
    void setDestinationCrs( QgsCoordinateReferenceSystem crs );

    QgsVectorLayer *destinationLayer() const;
    void setDestinationLayer( QgsVectorLayer *newDestinationLayer );

  public slots:

    void onMapSettingsUpdated();

  signals:

    void qgsProjectChanged( QgsProject *qgsProject );
    void mapSettingsChanged( QgsQuickMapSettings *mapSettings );

    void centerPositionChanged( QPointF centerPosition );

    void snappedPositionChanged( QgsPoint snappedPosition );

    void useSnappingChanged( bool useSnapping );

    void snappedChanged( bool snapped );

    void snapTypeChanged( const SnapUtils::SnapType &snapType );

    void destinationLayerChanged( QgsVectorLayer *destinationLayer );

  private:
    void setupSnapping();

    QgsProject *mQgsProject = nullptr;
    QgsQuickMapSettings *mMapSettings = nullptr;
    QgsSnappingUtils mSnappingUtils;
    QPointF mCenterPosition = QPointF( -1, -1 );
    QgsPoint mSnappedPosition = QgsPoint( -1, -1 );
    bool mSnapped;
    SnapType mSnapType = SnapUtils::Vertex;
    bool mUseSnapping;
    QgsVectorLayer *mDestinationLayer = nullptr;
};

#endif // SNAPUTILS_H
