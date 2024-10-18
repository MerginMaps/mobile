/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "abstractmaptool.h"

AbstractMapTool::AbstractMapTool( QObject *parent )
  : QObject{parent}
{

}

AbstractMapTool::~AbstractMapTool() = default;

InputMapSettings *AbstractMapTool::mapSettings() const
{
  return mMapSettings;
}

void AbstractMapTool::setMapSettings( InputMapSettings *newMapSettings )
{
  if ( mMapSettings == newMapSettings )
    return;

  emit onAboutToChangeMapSettings();

  mMapSettings = newMapSettings;

  emit mapSettingsChanged( mMapSettings );
}
