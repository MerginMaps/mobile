import QtQuick 2.4
import QtQuick.Window 2.2
import qgis 1.0

import QtQuick.Controls 1.3

Window {
    visible: true

    width: 800
    height: 600

    Component.onCompleted: {
        Project.projectFile = "/home/peter/qmobile/repo/qgis-quick-components/data/test_project.qgs"
        canvas.engine.layers = Project.layers
        canvas.engine.view.center = canvas.engine.fullExtent()
    }

    MapCanvas {
        id: canvas
        anchors.fill: parent

        engine.destinationCRS: "EPSG:4326" //"EPSG:3857"  // pseudo Mercator projection

        PositionAccuracy { }
        PositionMarker { }

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
                onClicked: {
                    canvas.view.center = PositionEngine.position
                    canvas.view.mupp = canvas.engine.scale2mupp(500000)
                }
            }
        }
    }

    IdentifyBar {
        id: identifyBar
        width: parent.width

    }

}
