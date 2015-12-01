import QtQuick 2.0

Rectangle {

    property var canvas: parent // assuming that MapCanvas is the parent node by default

    property point position  // location in map coords - should be set by clients

    property point _location  // position within canvas' image: computed automatically
    onPositionChanged: updateLocation()
    onCanvasChanged: updateLocation()

    Connections {
        target: canvas.view
        onChanged: updateLocation()
    }

    function updateLocation() {
        if (canvas)
            _location = canvas.view.mapToDisplay(position)
    }

    color: "red"

    width: 12
    height: width
    radius: width/2

    x: _location.x - width/2
    y: _location.y - height/2
}

