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
 * File: qgsquickmaptransform.h by (C) 2014 by Matthias Kuhn
 */

#ifndef INPUTMAPTRANSFORM_H
#define INPUTMAPTRANSFORM_H

#include <QQuickItem>
#include <QMatrix4x4>

#include "inputmapsettings.h"

/**
 * \brief The InputMapTransform is transformation that can be attached to any QQuickItem.
 *
 * If the item is based on the map coordinates, InputMapTransform will
 * transform it to the device coordinates based on the attached map settings.
 *
 * \note QML Type: MapTransform
 */
class InputMapTransform : public QQuickTransform
{
    Q_OBJECT

    /**
     * Associated map settings. Should be initialized before the first use from mapcanvas map settings.
     */
    Q_PROPERTY( InputMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )

  public:
    //! Creates a new map transform
    InputMapTransform() = default;
    ~InputMapTransform() = default;

    /**
     * Applies transformation based on current map settings to a matrix.
     *
     * Also optimize resulting matrix after transformation
     * \param matrix Matrix to be transformed
     */
    void applyTo( QMatrix4x4 *matrix ) const;

    //! \copydoc InputMapTransform::mapSettings
    InputMapSettings *mapSettings() const;

    //! \copydoc InputMapTransform::mapSettings
    void setMapSettings( InputMapSettings *mapSettings );

  signals:
    //! \copydoc InputMapTransform::mapSettings
    void mapSettingsChanged();

  private slots:
    void updateMatrix();

  private:
    InputMapSettings *mMapSettings = nullptr; // not owned
    QMatrix4x4 mMatrix;
};

#endif // INPUTMAPTRANSFORM_H
