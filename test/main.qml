import QtQuick 2.4
import QtQuick.Window 2.2
import qgis 1.0

import QtQuick.Controls 1.3

Window {
    visible: true

    width: 800
    height: 600

    Component.onCompleted: {
        Project.projectFile = "/home/martin/qgis/qgis-mobile/data/test_project.qgs"
        canvas.engine.layers = Project.layers
        canvas.engine.extent = canvas.engine.fullExtent()

        // add raster background
        ml.source = "/home/martin/ne_50m.tif"
        canvas.engine.layers.push(ml.layerId)
    }

    MapCanvas {
        id: canvas
        anchors.fill: parent

        engine.destinationCRS: "EPSG:4326" //"EPSG:3857"  // pseudo Mercator projection

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
            color: "white"

            // one more inner circle... just for fun!
            Rectangle {
                anchors.centerIn: positionMarker
                width: positionMarker.width - 2
                height: width
                radius: width/2
                color: "#43ade9"
            }
        }

        onClicked: {
            engine.identifyPoint(Qt.point(x,y))
        }

        Connections {
            target: canvas.engine
            onIdentifyResultChanged: {
                var res = canvas.engine.identifyResult;
                var hasData = res["__layer__"] !== "__none__"
                identifyBar.state = hasData ? "on" : "off"

                if (res["__layer__"] !== "points") {
                    identifyBar.content.textLine1 = res["Name"]
                    identifyBar.content.textLine2 = (res["__layer__"] === "polys" ? "Polygon" : "Linestring");
                } else {
                    identifyBar.content.textLine1 = res["Class"]
                    identifyBar.content.textLine2 = "Airplane";
                }
            }
        }
    }

    MapLayer {
        id: ml
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

    Rectangle {

        anchors.top: canvas.top
        anchors.left: canvas.left
        anchors.margins: 8

        radius: width/2

        color:  mousearea.pressed ? "#44000000" : "#88000000"
        width: btnLocation.width * 1.2
        height: width

        Image {
            id: btnLocation
            anchors.centerIn: parent
            source: "qrc:/icons/location.png"
            MouseArea {
                id: mousearea
                anchors.fill: btnLocation
                onClicked: canvas.engine.zoomToPoint(PositionEngine.position.x, PositionEngine.position.y, 500000)
            }
        }
    }

    IdentifyBar {
        id: identifyBar
        width: parent.width

    }

}
