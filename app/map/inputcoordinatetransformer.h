/**************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * The source code forked from https://github.com/qgis/QGIS on 25th Nov 2022
 * File: qgsquickcoordinatetransformer.h by (C) 2017 by Matthias Kuhn
 */

#ifndef INPUTCOORDINATETRANSFORMER_H
#define INPUTCOORDINATETRANSFORMER_H

#include <QObject>

#include "qgspoint.h"

#include "qgscoordinatetransformcontext.h"
#include "qgscoordinatereferencesystem.h"
#include "qgscoordinatetransform.h"
#include "qgspoint.h"

/**
 * \brief Helper class for transform of coordinates (QgsPoint) to a different coordinate reference system.
 *
 * It requires connection of transformation context from mapSettings, source position and source CRS to
 * calculate projected position in desired destination CRS.
 *
 * \note QML Type: CoordinateTransformer
 */
class InputCoordinateTransformer : public QObject
{
    Q_OBJECT

    //! Projected (destination) position (in destination CRS)
    Q_PROPERTY( QgsPoint projectedPosition READ projectedPosition NOTIFY projectedPositionChanged )

    //! Source position  (in source CRS)
    Q_PROPERTY( QgsPoint sourcePosition READ sourcePosition WRITE setSourcePosition NOTIFY sourcePositionChanged )

    //! Destination CRS
    Q_PROPERTY( QgsCoordinateReferenceSystem destinationCrs READ destinationCrs WRITE setDestinationCrs NOTIFY destinationCrsChanged )

    //! Source CRS, default 4326
    Q_PROPERTY( QgsCoordinateReferenceSystem sourceCrs READ sourceCrs WRITE setSourceCrs NOTIFY sourceCrsChanged )

    //! Transformation context, can be set from InputMapSettings::transformContext()
    Q_PROPERTY( QgsCoordinateTransformContext transformContext READ transformContext WRITE setTransformContext NOTIFY transformContextChanged )

  public:
    //! Creates new coordinate transformer
    explicit InputCoordinateTransformer( QObject *parent = nullptr );

    //!\copydoc InputCoordinateTransformer::projectedPosition
    QgsPoint projectedPosition() const;

    //!\copydoc InputCoordinateTransformer::sourcePosition
    QgsPoint sourcePosition() const;

    //!\copydoc InputCoordinateTransformer::sourcePosition
    void setSourcePosition( const QgsPoint &sourcePosition );

    //!\copydoc InputCoordinateTransformer::destinationCrs
    QgsCoordinateReferenceSystem destinationCrs() const;

    //!\copydoc InputCoordinateTransformer::destinationCrs
    void setDestinationCrs( const QgsCoordinateReferenceSystem &destinationCrs );

    //!\copydoc InputCoordinateTransformer::sourceCrs
    QgsCoordinateReferenceSystem sourceCrs() const;

    //!\copydoc InputCoordinateTransformer::sourceCrs
    void setSourceCrs( const QgsCoordinateReferenceSystem &sourceCrs );

    //!\copydoc InputCoordinateTransformer::transformContext
    void setTransformContext( const QgsCoordinateTransformContext &context );

    //!\copydoc InputCoordinateTransformer::transformContext
    QgsCoordinateTransformContext transformContext() const;

  signals:
    //!\copydoc InputCoordinateTransformer::projectedPosition
    void projectedPositionChanged();

    //!\copydoc InputCoordinateTransformer::sourcePosition
    void sourcePositionChanged();

    //!\copydoc InputCoordinateTransformer::destinationCrs
    void destinationCrsChanged();

    //!\copydoc InputCoordinateTransformer::sourceCrs
    void sourceCrsChanged();

    //!\copydoc InputCoordinateTransformer::transformContext
    void transformContextChanged();

  private:
    void updatePosition();

    QgsPoint mProjectedPosition;
    QgsPoint mSourcePosition;
    QgsCoordinateTransform mCoordinateTransform;
};

#endif // INPUTCOORDINATETRANSFORMER_H
