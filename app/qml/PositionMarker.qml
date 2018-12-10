import QtQuick 2.3
import QtQuick.Controls 2.2
import QtQml 2.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import "."

Item {
    id: positionMarker
    property int size: InputStyle.rowHeightHeader/2
    property QgsQuick.PositionKit positionKit
    property color baseColor: InputStyle.highlightColor
    property bool withAccuracy: true
    // animated position
    property real animatedPosX
    property real animatedPosY

    function changePos() {
        var posNext = positionKit.mapSettings.coordinateToScreen(positionKit.projectedPosition)

        animateX.from = animatedPosX
        animateX.to = posNext.x
        animateX.restart()

        animateY.from = animatedPosY
        animateY.to = posNext.y
        animateY.restart()
    }

    Component.onCompleted: {
        animatedPosX =  positionKit.projectedPosition.x
        animatedPosY = positionKit.projectedPosition.y
    }

    NumberAnimation {
        id: animateX
        target: positionMarker
        properties: "animatedPosX"
        duration: 1000
        easing {type: Easing.Linear}
    }

    NumberAnimation {
        id: animateY
        target: positionMarker
        properties: "animatedPosY"
        duration: 1000
        easing {type: Easing.Linear}
    }


    Rectangle {
        id: accuracyIndicator
        visible: withAccuracy &&
                 positionKit.hasPosition &&
                 (positionKit.accuracy > 0) &&
                 (accuracyIndicator.width > positionMarker.size / 2.0)
        x: animatedPosX - width/2
        y: animatedPosY - height/2
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
        rotation: positionKit.direction
        transformOrigin: Item.Bottom
        width: positionMarker.size * 2
        height: width
        smooth: true
        visible: positionKit.hasPosition && positionKit.direction >= 0
        x: animatedPosX - width/2
        y: animatedPosY - (height * 1)
    }

    Image {
        id: navigation
        source: positionKit.hasPosition ? "gps_marker_position.svg" : "gps_marker_no_position.svg"
        fillMode: Image.PreserveAspectFit
        width: positionMarker.size
        height: width
        smooth: true
        x: animatedPosX - width/2
        y: animatedPosY - height/2
    }
}

