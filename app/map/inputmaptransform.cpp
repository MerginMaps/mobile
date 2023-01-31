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
 * File: qgsquickmaptransform.cpp by (C) 2014 by Matthias Kuhn
 */

#include "inputmaptransform.h"

void InputMapTransform::applyTo( QMatrix4x4 *matrix ) const
{
  *matrix *= mMatrix;
  matrix->optimize();
}

InputMapSettings *InputMapTransform::mapSettings() const
{
  return mMapSettings;
}

void InputMapTransform::setMapSettings( InputMapSettings *mapSettings )
{
  if ( mapSettings == mMapSettings )
    return;

  if ( mMapSettings )
    disconnect( mMapSettings, &InputMapSettings::visibleExtentChanged, this, &InputMapTransform::updateMatrix );

  mMapSettings = mapSettings;

  if ( mMapSettings )
    connect( mMapSettings, &InputMapSettings::visibleExtentChanged, this, &InputMapTransform::updateMatrix );

  updateMatrix();

  emit mapSettingsChanged();
}

void InputMapTransform::updateMatrix()
{
  QMatrix4x4 matrix;
  float scaleFactor = static_cast<float>( ( 1.0 / mMapSettings->mapUnitsPerPixel() ) / mMapSettings->devicePixelRatio() );

  matrix.scale( scaleFactor, -scaleFactor );
  matrix.translate( static_cast<float>( -mMapSettings->visibleExtent().xMinimum( ) ),
                    static_cast<float>( -mMapSettings->visibleExtent().yMaximum() ) );

  mMatrix = matrix;
  update();
}
