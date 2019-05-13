import QtQuick 2.3
import QtQuick.Controls 2.2
import QtQml 2.2
import QtGraphicalEffects 1.0
import QtSensors 5.0
import QgsQuick 0.1 as QgsQuick
import "."

Item {
    id: positionMarker
    property int size: InputStyle.rowHeightHeader/2
    property QgsQuick.PositionKit positionKit
    property color baseColor: InputStyle.highlightColor
    property bool withAccuracy: true
    property int interval: 100 // Interval of direction marker updates in ms
    property real threshold: 3 // threshold used to minimized direction marker updates (otherwise shaking)
    property real direction

    Timer {
      interval: positionMarker.interval; running: true; repeat: true
      onTriggered: {
        var newDirection = compass.reading ? compass.reading.azimuth : positionKit.direction
        if (Math.abs(positionMarker.direction - newDirection) > positionMarker.threshold) {
          positionMarker.direction = newDirection
        }
      }
    }

    Compass {
      id: compass
      active: true
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
        rotation: positionMarker.direction
        transformOrigin: Item.Bottom
        width: positionMarker.size * 2
        height: width
        smooth: true
        visible: positionKit.hasPosition
        x: positionKit.screenPosition.x - width/2
        y: positionKit.screenPosition.y - (height * 1)
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
