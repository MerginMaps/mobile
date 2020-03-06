import QtQuick 2.3
import QtQuick.Controls 2.2
import QtQml 2.2
import QtGraphicalEffects 1.0
import QtSensors 5.11
import QgsQuick 0.1 as QgsQuick
import "."

Item {
    id: positionMarker
    property int size: InputStyle.rowHeightHeader/2
    property QgsQuick.PositionKit positionKit
    property color baseColor: InputStyle.highlightColor
    property bool withAccuracy: true
    property int interval: 200 // Interval of direction marker updates in ms
    property real threshold: 3 // threshold used to minimized direction marker updates (otherwise shaking)
    property real speedLimit: 15  // Over speed limit, directions depends on direction of movement
    property real direction: 0
    property real groundSpeed

    Timer {
      interval: positionMarker.interval; running: true; repeat: true
      onTriggered: {
        positionMarker.groundSpeed = __inputUtils.groundSpeedFromSource(positionKit)
        var newDirection = compass.reading && groundSpeed < speedLimit ? compass.reading.azimuth : positionKit.direction
        var deltaAbsDirection = Math.abs(positionMarker.direction - newDirection)
        if (deltaAbsDirection > positionMarker.threshold) {
          positionMarker.direction = newDirection + compass.userOrientation
        }
       }
    }

    OrientationSensor {
          id: orientationSensor
          active: true

          onReadingChanged: {
            if (reading.orientation == OrientationReading.TopUp) {
                compass.userOrientation = 0;
            } else if (reading.orientation == OrientationReading.TopDown) {
                compass.userOrientation = 180;
            } else if (reading.orientation == OrientationReading.RightUp) {
                compass.userOrientation = 270;
            } else if (reading.orientation == OrientationReading.LeftUp) {
                compass.userOrientation = 90;
            }
          }
      }

    Compass {
      id: compass
      active: true
      axesOrientationMode: Sensor.AutomaticOrientation
      skipDuplicates: true
      bufferSize: 100
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
