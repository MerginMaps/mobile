import QtQuick 2.4
import QtQuick.Window 2.2
import qgis 1.0

Window {
    visible: true

    width: 800
    height: 600

    MapCanvas {
        id: canvas
        anchors.fill: parent

        engine.destinationCRS: "EPSG:4326" //"EPSG:3857"  // pseudo Mercator projection

        engine.layers: [ml.layerId]

        engine.extent: engine.layerExtent(ml.layerId)

        VertexMarker {
            position: PositionEngine.position
            visible: PositionEngine.hasPosition && PositionEngine.accuracy != -1

            opacity: 0.2
            color: "#43ade9"
            width: 2*PositionEngine.accuracy / canvas.engine.metersPerPixel
        }

        VertexMarker {
            id: positionMarker

            position: PositionEngine.position
            visible: PositionEngine.hasPosition
            color: "#25252a"

            // one more inner circle... just for fun!
            Rectangle {
                anchors.centerIn: positionMarker
                width: positionMarker.width - 2
                height: width
                radius: width/2
                color: "#43ade9"
            }
        }

    }

    MapLayer {
        id: ml

        source: "/home/martin/ne_50m.tif"
        provider: "gdal"
    }

    ScaleBar {
        id: scaleBar

        engine: canvas.engine

        anchors.bottom: canvas.bottom
        anchors.right: canvas.right
        anchors.bottomMargin: 8
        anchors.rightMargin: 8
    }
}
