import QtQuick 2.4
import qgis 1.0

VertexMarkerBase {
    position: canvas.engine.wgs84ToMap(PositionEngine.position)
    visible: PositionEngine.hasPosition && PositionEngine.accuracy != -1

    width: accuracyRect.width
    height: accuracyRect.height
    markerHotSpot: Qt.point(accuracyRect.width/2, accuracyRect.height/2)

    Rectangle {
        id: accuracyRect
        opacity: 0.2
        color: "#43ade9"
        width: 2*PositionEngine.accuracy / canvas.engine.metersPerPixel
        height: width
        radius: width/2
    }
}
