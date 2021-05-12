/***************************************************************************
  maptransform.h
  --------------------------------------
  Date                 : 27.12.2014
  Copyright            : (C) 2014 by Matthias Kuhn
  Email                : matthias (at) opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MAPTRANSFORM_H
#define MAPTRANSFORM_H

#include <QQuickItem>
#include <QMatrix4x4>



class MapSettings;

/**
 * \ingroup quick
 * \brief The MapTransform is transformation that can be attached to any QQuickItem.
 *
 * If the item is based on the map coordinates, MapTransform will
 * transform it to the device coordinates based on the attached map settings.
 *
 * \note QML Type: MapTransform
 *
 * \since QGIS 3.4
 */
class  MapTransform : public QQuickTransform
{
    Q_OBJECT

    /**
     * Associated map settings. Should be initialized before the first use from mapcanvas map settings.
     */
    Q_PROPERTY( MapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )

  public:
    //! Creates a new map transform
    MapTransform() = default;
    ~MapTransform() = default;

    /**
     * Applies transformation based on current map settings to a matrix.
     *
     * Also optimize resulting matrix after transformation
     * \param matrix Matrix to be transformed
     */
    void applyTo( QMatrix4x4 *matrix ) const;

    //! \copydoc MapTransform::mapSettings
    MapSettings *mapSettings() const;

    //! \copydoc MapTransform::mapSettings
    void setMapSettings( MapSettings *mapSettings );

  signals:
    //! \copydoc MapTransform::mapSettings
    void mapSettingsChanged();

  private slots:
    void updateMatrix();

  private:
    MapSettings *mMapSettings = nullptr; // not owned
    QMatrix4x4 mMatrix;
};

#endif // MAPTRANSFORM_H
