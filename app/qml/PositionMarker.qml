import QtQuick 2.3
import QtQuick.Controls 2.2
import QtQml 2.2
import QtGraphicalEffects 1.0
import QtSensors 5.11
import QgsQuick 0.1 as QgsQuick
import "."
import lc 1.0

Item {
    id: positionMarker
    property int size: InputStyle.rowHeightHeader/2
    property QgsQuick.PositionKit positionKit
    property color baseColor: InputStyle.highlightColor
    property bool withAccuracy: true
    property real direction: -1

    onPositionKitChanged: positionDirection.positionKit = positionMarker.positionKit

    PositionDirection {
      id: positionDirection
      onDirectionChanged: direction.rotation = positionDirection.direction
    }

    Rectangle {
        id: accuracyIndicator
        visible: withAccuracy &&
                 positionKit.hasPosition &&
                 (positionKit.accuracy > 0) &&
                 (accuracyIndicator.width > positionMarker.size / 2.0)
        x: positionKit.screenPosition.x - width/2
        y: positionKit.screenPosition.y - height/2
        width:positionKit.screenAccuracy
        height: accuracyIndicator.width
        color: InputStyle.highlightColor
        radius: width*0.5
        opacity: 0.2
    }

    Image {
        id: direction
        source: "gps_direction.svg"
        fillMode: Image.PreserveAspectFit
        rotation: positionDirection.direction
        transformOrigin: Item.Bottom
        width: positionMarker.size * 2
        height: width
        smooth: true
        visible: positionKit.hasPosition && positionDirection.direction > positionDirection.MIN_INVALID_DIRECTION
        x: positionKit.screenPosition.x - width/2
        y: positionKit.screenPosition.y - (height * 1)

        Behavior on rotation { RotationAnimation { properties: "rotation"; direction: RotationAnimation.Shortest; duration: 500 }}
    }

    Image {
        id: navigation
        source: positionKit.hasPosition ? "gps_marker_position.svg" : "gps_marker_no_position.svg"
        fillMode: Image.PreserveAspectFit
        width: positionMarker.size
        height: width
        smooth: true
        x: positionKit.screenPosition.x - width/2
        y: positionKit.screenPosition.y - height/2
    }
}
