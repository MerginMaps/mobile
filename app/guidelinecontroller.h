/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GUIDELINECONTROLLER_H
#define GUIDELINECONTROLLER_H

#include <QObject>
#include <qglobal.h>

#include "qgsgeometry.h"
#include "qgsquickmapsettings.h"

class GuidelineController : public QObject
{
    Q_OBJECT

    // input properties (geometry of real feature and curent crosshair position in map CRS)
    Q_PROPERTY( QgsGeometry realGeometry READ realGeometry WRITE setRealGeometry NOTIFY realGeometryChanged )
    Q_PROPERTY( QgsPoint crosshairPosition READ crosshairPosition WRITE setCrosshairPosition NOTIFY crosshairPositionChanged )

    // output properties (real geometry + crosshair position ) in map CRS
    Q_PROPERTY( QgsGeometry guidelineGeometry READ guidelineGeometry WRITE setGuidelineGeometry NOTIFY guidelineGeometryChanged )

  public:
    explicit GuidelineController( QObject *parent = nullptr );

    const QgsGeometry &guidelineGeometry() const;
    void setGuidelineGeometry( const QgsGeometry &newGuidelineGeometry );

    QgsPoint crosshairPosition() const;
    void setCrosshairPosition( QgsPoint newCrosshairPosition );

    const QgsGeometry &realGeometry() const;
    void setRealGeometry( const QgsGeometry &newRealGeometry );

  signals:

    void guidelineGeometryChanged( const QgsGeometry &guidelineGeometry );
    void crosshairPositionChanged( QgsPoint crosshairPosition );

    void realGeometryChanged( const QgsGeometry &realGeometry );

  private:
    void buildGuideline();

    QgsGeometry mGuidelineGeometry;
    QgsPoint mCrosshairPosition;
    QgsGeometry mRealGeometry;
};

#endif // GUIDELINECONTROLLER_H
