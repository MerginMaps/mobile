/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.3
import QtQuick.Controls 2.2
import QtQml 2.2
import QtGraphicalEffects 1.0
import "../"
import lc 1.0
import QgsQuick 0.1 as QgsQuick

Item {
  id: positionMarker
  property color baseColor: InputStyle.highlightColor
  property Compass compass
  property PositionKit positionKit
  property int size: InputStyle.rowHeightHeader / 2
  property bool withAccuracy: true

  onCompassChanged: positionDirection.compass = positionMarker.compass
  onPositionKitChanged: positionDirection.positionKit = positionMarker.positionKit

  PositionDirection {
    id: positionDirection
  }
  Rectangle {
    id: accuracyIndicator
    color: InputStyle.highlightColor
    height: accuracyIndicator.width
    opacity: 0.2
    radius: width * 0.5
    visible: withAccuracy && positionKit.hasPosition && (positionKit.accuracy > 0) && (accuracyIndicator.width > positionMarker.size / 2.0)
    width: positionKit.screenAccuracy
    x: positionKit.screenPosition.x - width / 2
    y: positionKit.screenPosition.y - height / 2
  }
  Image {
    id: direction
    fillMode: Image.PreserveAspectFit
    height: width
    rotation: positionDirection.direction
    smooth: true
    source: InputStyle.gpsDirectionIcon
    transformOrigin: Item.Bottom
    visible: positionKit.hasPosition && positionDirection.hasDirection
    width: positionMarker.size * 2
    x: positionKit.screenPosition.x - width / 2
    y: positionKit.screenPosition.y - (height * 1)

    Behavior on rotation  {
      RotationAnimation {
        direction: RotationAnimation.Shortest
        duration: 500
        properties: "rotation"
      }
    }
  }
  Image {
    id: navigation
    fillMode: Image.PreserveAspectFit
    height: width
    smooth: true
    source: positionKit.hasPosition ? InputStyle.gpsMarkerPositionIcon : InputStyle.gpsMarkerNoPositionIcon
    width: positionMarker.size
    x: positionKit.screenPosition.x - width / 2
    y: positionKit.screenPosition.y - height / 2
  }
}
