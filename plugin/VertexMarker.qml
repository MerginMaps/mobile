import QtQuick 2.0

VertexMarkerBase {

    width: marker.width
    height: marker.height

    property alias marker: marker

    Rectangle {
        id: marker
        color: "red"
        width: 12
        height: width
        radius: width/2
    }

    markerHotSpot: Qt.point(marker.width/2, marker.height/2)
}
