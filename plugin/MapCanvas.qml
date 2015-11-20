import QtQuick 2.0
import qgis 1.0

Rectangle {
    id: canvas
    width: 100
    height: 100

    property alias engine: mapEngine
    property point panOffset: Qt.point(mapImage.x, mapImage.y)
    property real pinchScale: mapImage.width/width

    // emitted on change of map image, not directly after change of engine settings
    // so that items using it change their values at the same time (to avoid flicker)
    signal engineSettingsChanged

    // emitted when a single point is clicked
    signal clicked(real x, real y)

    color: "white"

    MapEngine {
        id: mapEngine

        imageSize: Qt.size( canvas.width, canvas.height )

        onMapImageChanged: {
            mapImage.x = 0
            mapImage.y = 0
            mapImage.width = imageSize.width
            mapImage.height = imageSize.height

            canvas.engineSettingsChanged()
        }
    }

    MapImage {
        id: mapImage
        mapEngine: mapEngine
    }


    PinchArea {
        id: pinchArea
        anchors.fill: canvas

        // initial values
        property real iw
        property real ih

        onPinchStarted: {
            iw = mapImage.width
            ih = mapImage.height
        }
        onPinchUpdated: {
            mapImage.width  = iw * pinch.scale
            mapImage.height = ih * pinch.scale
            mapImage.x = -(iw*pinch.scale-iw)/2
            mapImage.y = -(ih*pinch.scale-ih)/2
        }
        onPinchFinished: {
            mapEngine.scale(pinch.scale)
        }

    // mouse area needs to be inside pinch area in order to have
    // both pinch and mouse area working together
    MouseArea {
        anchors.fill: pinchArea

        property bool panning: false
        property point panStart

        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                panning = true
                panStart = Qt.point( mouse.x, mouse.y )
            }
        }

        onReleased: {
            if (mouse.button == Qt.LeftButton) {
                panning = false
                if (Math.abs(panStart.x - mouse.x) == 0 && Math.abs(panStart.y - mouse.y) == 0)
                    canvas.clicked(mouse.x, mouse.y)
                else
                    mapEngine.move(panStart.x, panStart.y, mouse.x, mouse.y)
            }
        }

        onPositionChanged: {
            if (panning) {
                mapImage.x = mouse.x - panStart.x
                mapImage.y = mouse.y - panStart.y
            }
        }

        function temporaryZoom(d) {
            var dw = mapImage.width*d
            var dh = mapImage.height*d
            mapImage.x -= dw/2
            mapImage.y -= dh/2
            mapImage.width += dw
            mapImage.height += dh

            zoomRefreshTimer.restart()
        }

        Timer {
             id: zoomRefreshTimer
             interval: 100
             onTriggered: mapEngine.scale(mapImage.width / canvas.width)
         }

        onWheel: {
            if (wheel.angleDelta.y > 0)
                temporaryZoom(0.1)
            else if (wheel.angleDelta.y < 0)
                temporaryZoom(-0.1)
        }

        onDoubleClicked: {
            if (mouse.button == Qt.RightButton)
                temporaryZoom(-0.1)
            else
                temporaryZoom(0.1)
        }

    }

    }

}
