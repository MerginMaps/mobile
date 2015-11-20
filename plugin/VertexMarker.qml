import QtQuick 2.0

Rectangle {

    property var canvas: parent // assuming that MapCanvas is the parent node by default

    property point position  // location in WGS 84 coords - should be set by clients

    property point location  // position within canvas' image: computed automatically
    onPositionChanged: updateLocation()
    onCanvasChanged: updateLocation()

    Connections {
        target: canvas
        onEngineSettingsChanged: updateLocation()
    }

    function updateLocation() {
        if (canvas)
            location = canvas.engine.convertWgs84ToImageCoords(position)
    }

    color: "red"

    width: 12
    height: width
    radius: width/2

    x: canvas.pinchScale*location.x - width/2  + canvas.panOffset.x
    y: canvas.pinchScale*location.y - height/2 + canvas.panOffset.y
}

