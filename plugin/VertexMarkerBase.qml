import QtQuick 2.0

Item {

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

    //property var marker //: Item {}

    property point markerHotSpot: Qt.point(0,0)

    //width: marker.width
    //height: marker.height

    x: _location.x - markerHotSpot.x
    y: _location.y - markerHotSpot.y
}

