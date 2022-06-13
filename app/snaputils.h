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
    Q_PROPERTY( QgsQuickMapSettings *mapSettings READ mapSettings WRITE mapSettings NOTIFY mapSettingsChanged )
    Q_PROPERTY( QPoint centerPosition READ centerPosition WRITE centerPosition NOTIFY centerPositionChanged )
    Q_PROPERTY( bool snapped READ snapped WRITE setSnapped NOTIFY snappedChanged )
    Q_PROPERTY( QPoint snappedPosition READ snappedPosition WRITE setSnappedPosition NOTIFY snappedPositionChanged )

  public:
    SnapUtils( QObject *parent = nullptr );

    QgsProject *qgsProject() const;
    void setQgsProject( QgsProject *newQgsProject );

    QgsQuickMapSettings *mapSettings() const;
    void mapSettings( QgsQuickMapSettings *newMapSettings );

    Q_INVOKABLE void getsnap( QPoint mapPoint );

    void setup();

    QPoint centerPosition() const;
    void centerPosition( QPoint newCenterPosition );

    QPoint snappedPosition() const;
    void setSnappedPosition( QPoint newSnappedPosition );

    bool snapped() const;
    void setSnapped( bool newSnapped );

  signals:

    void qgsProjectChanged( QgsProject *qgsProject );
    void mapSettingsChanged( QgsQuickMapSettings *mapSettings );

    void centerPositionChanged( QPoint centerPosition );

    void snappedPositionChanged( QPoint snappedPosition );

    void snappedChanged( bool snapped );

  private:
    QgsProject *mQgsProject;
    QgsQuickMapSettings *mMapSettings;
    QgsSnappingUtils mSnappingUtils;
    QPoint mCenterPosition = QPoint( -1, -1 );
    QPoint mSnappedPosition = QPoint( -1, -1 );
    bool mSnapped;
};

#endif // SNAPUTILS_H
