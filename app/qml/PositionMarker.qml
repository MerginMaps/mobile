import QtQuick 2.3
import QtQuick.Controls 2.2
import QtQml 2.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import "."

Item {
    id: positionMarker
    property int size: 300 * QgsQuick.Utils.dp
    property QgsQuick.PositionKit positionKit
    property color baseColor: InputStyle.highlightColor
    property color unavailableColor: "gray"
    property bool withAccuracy: true


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

    Rectangle {
      id: lastPosition
      visible: !(positionKit.hasPosition)
      property int borderWidth: 10 * QgsQuick.Utils.dp
      width: borderWidth + 20 * QgsQuick.Utils.dp
      height: width
      color: "transparent"
      border.color: InputStyle.panelBackgroundDark
      border.width: borderWidth
      radius: width*0.5
      antialiasing: true
      x: positionKit.screenPosition.x - width/2
      y: positionKit.screenPosition.y - height/2
      z: navigation.z + 1
    }

    Image {
        id: navigation
        source: "gps.svg"
        fillMode: Image.PreserveAspectFit
        rotation: positionKit.direction
        width: positionMarker.size
        height: width
        smooth: true
        x: positionKit.screenPosition.x - width/2
        y: positionKit.screenPosition.y - height/2
        visible: positionKit.hasPosition
    }
}

