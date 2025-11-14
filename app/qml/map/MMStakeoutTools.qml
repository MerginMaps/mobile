/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import mm 1.0 as MM
import MMInput

Item {
  id: root

  /*required*/ property var map
  /*required*/ property var target

  property real mapExtentOffset: 0

  MMHighlight {
    id: highlight

    height: root.map.height
    width: root.map.width

    lineColor: __style.forestColor

    mapSettings: root.map.mapSettings
  }

  MM.MapPosition {
    id: mapPositioning

    mapSettings: map.mapSettings
    positionKit: PositionKit
    onMapPositionChanged: updateStakeout()
  }

  Component.onCompleted: updateStakeout()

  function updateStakeout() {
    highlight.geometry = __inputUtils.stakeoutGeometry( mapPositioning.mapPosition, target, map.mapSettings )
  }
}
