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
    positionKit: __positionKit
    onScreenPositionChanged: updateStakeout()
  }

  Connections {
    target: map
    function onUserInteractedWithMap() {
      internal.shouldAutoFollowStakoutPath = false
    }
  }

  Component.onCompleted: updateStakeout()

  QtObject {
    id: internal

    // Determines if canvas is auto centered to stakeout line
    property bool shouldAutoFollowStakoutPath: true

    onShouldAutoFollowStakoutPathChanged: updateStakeout()
  }

  function updateStakeout() {
    if ( internal.shouldAutoFollowStakoutPath )
    {
      map.mapSettings.extent = __inputUtils.stakeoutPathExtent( mapPositioning, target, map.mapSettings, mapExtentOffset )
    }
    highlight.geometry = __inputUtils.stakeoutGeometry( mapPositioning.mapPosition, target, map.mapSettings )
  }

  function autoFollow() {
    internal.shouldAutoFollowStakoutPath = true
  }
}
