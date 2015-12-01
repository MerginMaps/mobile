import QtQuick 2.4
import qgis 1.0

VertexMarkerBase {
    position: canvas.engine.wgs84ToMap(PositionEngine.position)
    visible: PositionEngine.hasPosition

    width: outerRect.width
    height: outerRect.height
    markerHotSpot: Qt.point(outerRect.width/2, outerRect.height/2)

    Rectangle {
        id: outerRect
        width: 15
        height: width
        radius: width/2
        color: "white"
    }

    // one more inner circle... just for fun!
    Rectangle {
        anchors.centerIn: outerRect
        width: outerRect.width - 4
        height: width
        radius: width/2
        color: "#43ade9"
    }
}
