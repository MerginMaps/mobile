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

#include "qgsproject.h"
#include "qgsquickmapsettings.h"

#include "qgssnappingutils.h"

class SnapUtils : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QgsProject *qgsProject READ qgsProject WRITE setQgsProject NOTIFY qgsProjectChanged )
    Q_PROPERTY( QgsQuickMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )
    Q_PROPERTY( QPoint centerPosition READ centerPosition WRITE setCenterPosition NOTIFY centerPositionChanged )
    Q_PROPERTY( bool snapped READ snapped WRITE setSnapped NOTIFY snappedChanged )
    Q_PROPERTY( QPoint snappedPosition READ snappedPosition WRITE setSnappedPosition NOTIFY snappedPositionChanged )
    Q_PROPERTY( SnapType snapType READ snapType WRITE setSnapType NOTIFY snapTypeChanged )

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

    Q_INVOKABLE void getsnap( QPoint mapPoint );

    void setup();

    QPoint centerPosition() const;
    void setCenterPosition( QPoint newCenterPosition );

    QPoint snappedPosition() const;
    void setSnappedPosition( QPoint newSnappedPosition );

    bool snapped() const;
    void setSnapped( bool newSnapped );

    const SnapUtils::SnapType &snapType() const;
    void setSnapType( const SnapUtils::SnapType &newSnapType );

  signals:

    void qgsProjectChanged( QgsProject *qgsProject );
    void mapSettingsChanged( QgsQuickMapSettings *mapSettings );

    void centerPositionChanged( QPoint centerPosition );

    void snappedPositionChanged( QPoint snappedPosition );

    void snappedChanged( bool snapped );

    void snapTypeChanged( const SnapUtils::SnapType &snapType );

  private:
    QgsProject *mQgsProject = nullptr;
    QgsQuickMapSettings *mMapSettings = nullptr;
    QgsSnappingUtils mSnappingUtils;
    QPoint mCenterPosition = QPoint( -1, -1 );
    QPoint mSnappedPosition = QPoint( -1, -1 );
    bool mSnapped;
    SnapType mSnapType = SnapUtils::Vertex;
};

#endif // SNAPUTILS_H
