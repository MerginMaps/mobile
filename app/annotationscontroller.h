/***************************************************************************
    annotationscontroller.h
    ---------------------
    begin                : May 2025
    copyright            : (C) 2025 by Stefanos Natsis
    email                : uclaros at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ANNOTATIONSCONTROLLER_H
#define ANNOTATIONSCONTROLLER_H

#include <QObject>
#include <QPointF>
#include <QColor>

#include "qgsgeometry.h"

class QgsVectorLayer;
class InputMapSettings;

class AnnotationsController : public QObject
{
    Q_OBJECT

    Q_PROPERTY( InputMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )
    Q_PROPERTY( QgsGeometry highlightGeometry READ highlightGeometry NOTIFY highlightGeometryChanged )
    Q_PROPERTY( QColor activeColor READ activeColor WRITE setActiveColor NOTIFY activeColorChanged )
    Q_PROPERTY( bool canUndo READ canUndo NOTIFY canUndoChanged )
    Q_PROPERTY( bool eraserActive READ eraserActive WRITE setEraserActive NOTIFY eraserActiveChanged )

  public:
    explicit AnnotationsController( QObject *parent = nullptr );
    ~AnnotationsController();

    Q_INVOKABLE void updateHighlight( QPointF oldPoint, QPointF newPoint );

    Q_INVOKABLE void finishDigitizing();

    Q_INVOKABLE void undo();

    Q_INVOKABLE QStringList availableColors() const;

  signals:
    void highlightGeometryChanged();
    void activeColorChanged();
    void mapSettingsChanged();
    void canUndoChanged( bool canUndo );
    void eraserActiveChanged();

  private:
    QgsGeometry highlightGeometry() const;
    void clearHighlight();
    void setMapSettings( InputMapSettings *settings );
    InputMapSettings *mapSettings() const;
    bool canUndo() const;
    bool eraserActive() const;
    void setEraserActive( bool active );
    void setActiveColor( const QColor &color );
    QColor activeColor() const;

    InputMapSettings *mMapSettings = nullptr;
    QgsVectorLayer *mLayer = nullptr;

    QgsGeometry mHighlight;

    QgsGeometry mScreenPoints;
    QColor mColor;

    bool mEraserActive = false;
};

#endif // ANNOTATIONSCONTROLLER_H
