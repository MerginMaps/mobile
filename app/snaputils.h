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
#include "inputmapsettings.h"

#include "qgssnappingutils.h"

class SnapUtils : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QgsProject *qgsProject READ qgsProject WRITE setQgsProject NOTIFY qgsProjectChanged )
    Q_PROPERTY( InputMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )
    Q_PROPERTY( QPointF centerPosition READ centerPosition WRITE setCenterPosition NOTIFY centerPositionChanged )
    Q_PROPERTY( SnapType snapType READ snapType WRITE setSnapType NOTIFY snapTypeChanged )
    Q_PROPERTY( bool useSnapping READ useSnapping WRITE setUseSnapping NOTIFY useSnappingChanged )
    Q_PROPERTY( QgsVectorLayer *destinationLayer READ destinationLayer WRITE setDestinationLayer NOTIFY destinationLayerChanged )

    Q_PROPERTY( bool snapped READ snapped WRITE setSnapped NOTIFY snappedChanged )
    Q_PROPERTY( QgsPoint recordPoint READ recordPoint WRITE setRecordPoint NOTIFY recordPointChanged )

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

    InputMapSettings *mapSettings() const;
    void setMapSettings( InputMapSettings *newMapSettings );

    Q_INVOKABLE void getsnap();

    Q_INVOKABLE void clear();

    QPointF centerPosition() const;
    void setCenterPosition( QPointF newCenterPosition );

    QgsPoint recordPoint() const;
    void setRecordPoint( QgsPoint newRecordPoint );

    bool snapped() const;
    void setSnapped( bool newSnapped );

    bool useSnapping() const;
    void setUseSnapping( bool useSnapping );

    const SnapUtils::SnapType &snapType() const;
    void setSnapType( const SnapUtils::SnapType &newSnapType );

    QgsVectorLayer *destinationLayer() const;
    void setDestinationLayer( QgsVectorLayer *newDestinationLayer );

  public slots:

    void onMapSettingsUpdated();

  signals:

    void qgsProjectChanged( QgsProject *qgsProject );
    void mapSettingsChanged( InputMapSettings *mapSettings );

    void centerPositionChanged( QPointF centerPosition );

    void recordPointChanged( QgsPoint recordPoint );

    void useSnappingChanged( bool useSnapping );

    void snappedChanged( bool snapped );

    void snapTypeChanged( const SnapUtils::SnapType &snapType );

    void destinationLayerChanged( QgsVectorLayer *destinationLayer );

  private:
    void setupSnapping();
    void initializeRecordPosition();

    QgsSnappingUtils mSnappingUtils;

    QgsProject *mQgsProject = nullptr; // not owned
    InputMapSettings *mMapSettings = nullptr; // not owned
    QgsVectorLayer *mDestinationLayer = nullptr; // not owned

    QPointF mCenterPosition = QPointF( -1, -1 );
    QgsPoint mRecordPoint = QgsPoint( -1, -1 );

    bool mSnapped = false;
    bool mUseSnapping = false;
    SnapType mSnapType = SnapUtils::Vertex;
};

#endif // SNAPUTILS_H
