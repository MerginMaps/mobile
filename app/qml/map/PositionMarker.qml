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

import ".."
import lc 1.0

Item {
    id: positionMarker

    /*required*/ property MapPosition mapPosition
    property Compass compass

    property int size: InputStyle.rowHeightHeader/2
    property color baseColor: InputStyle.highlightColor
    property bool withAccuracy: true

    onCompassChanged: positionDirection.compass = positionMarker.compass

    PositionDirection {
      id: positionDirection

      positionKit: __positionKit
    }

    Rectangle {
        id: accuracyIndicator

        visible: withAccuracy &&
                 __positionKit.hasPosition &&
                 (__positionKit.horizontalAccuracy > 0) &&
                 (accuracyIndicator.width > positionMarker.size / 2.0)
        x: mapPosition.screenPosition.x - width/2
        y: mapPosition.screenPosition.y - height/2
        width: mapPosition.screenAccuracy
        height: accuracyIndicator.width
        color: InputStyle.highlightColor
        radius: width*0.5
        opacity: 0.2
    }

    Image {
        id: direction

        source: InputStyle.gpsDirectionIcon
        fillMode: Image.PreserveAspectFit
        rotation: positionDirection.direction
        transformOrigin: Item.Bottom
        width: positionMarker.size * 2
        height: width
        smooth: true
        visible: __positionKit.hasPosition && positionDirection.hasDirection
        x: mapPosition.screenPosition.x - width/2
        y: mapPosition.screenPosition.y - (height * 1)

        Behavior on rotation { RotationAnimation { properties: "rotation"; direction: RotationAnimation.Shortest; duration: 500 }}
    }

    Image {
        id: navigation

        source: __positionKit.hasPosition ? InputStyle.gpsMarkerPositionIcon : InputStyle.gpsMarkerNoPositionIcon
        visible: __positionKit.hasPosition
        fillMode: Image.PreserveAspectFit
        width: positionMarker.size
        height: width
        smooth: true
        x: mapPosition.screenPosition.x - width/2
        y: mapPosition.screenPosition.y - height/2
    }
}
