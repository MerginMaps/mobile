import QtQuick 2.0
import qgis 1.0

Rectangle {
    id: canvas
    implicitWidth: 256
    implicitHeight: 256

    property alias engine: mapEngine
    property real imgDx: mapImage.dx
    property real imgDy: mapImage.dy
    property real imgScale: mapImage.scale

    // emitted on change of map image, not directly after change of engine settings
    // so that items using it change their values at the same time (to avoid flicker)
    signal engineSettingsChanged

    // emitted when a single point is clicked
    signal clicked(real x, real y)

    // emitted when a single point is clicked for a longer time
    signal pressAndHold(real x, real y)

    color: "white"

    MapEngine {
        id: mapEngine

        imageSize: Qt.size( canvas.width, canvas.height )

        onMapImageChanged: {
            if (!mapImage.prev) {
                // initial
                mapImage.dx = 0
                mapImage.dy = 0
                mapImage.scale = 1
            } else {
                //console.debug("cur " + mapImage.dx + " " + mapImage.dy + " " + mapImage.scale)
                //console.debug("prev " + mapImage.prevDx + " " + mapImage.prevDy + " " + mapImage.prevScale)
                mapImage.dx -= mapImage.prevDx
                mapImage.dy -= mapImage.prevDy
                mapImage.scale /= mapImage.prevScale
                //console.debug("res " + mapImage.dx + " " + mapImage.dy + " " + mapImage.scale)
                if (mouseArea.panning) {
                    mouseArea.dxStart -= mapImage.prevDx
                    mouseArea.dyStart -= mapImage.prevDy
                }
                if (mouseArea.zooming)
                    mouseArea.wheelScaleStart /= mapImage.prevScale
                if (pinchArea.pinching) {
                    // TODO: not yet working correctly
                    pinchArea.startDx -= mapImage.prevDx
                    pinchArea.startDy -= mapImage.prevDy
                    pinchArea.startScale /= mapImage.prevScale
                }
            }
            //mapImage.setPrevious()
            canvas.engineSettingsChanged()
        }
    }

    MapImage {
        id: mapImage
        mapEngine: mapEngine

        property real dx: 0
        property real dy: 0
        property real scale: 1

        property bool prev: false
        property real prevDx
        property real prevDy
        property real prevScale

        // set the state of the image at the time another refresh request was made
        function setPrevious() {
            mapImage.prev = true
            mapImage.prevDx = mapImage.dx
            mapImage.prevDy = mapImage.dy
            mapImage.prevScale = mapImage.scale
        }

        transform: [
            Scale {
                xScale: mapImage.scale
                yScale: mapImage.scale
                origin.x: canvas.width/2
                origin.y: canvas.height/2
            },
            Translate {
                x: mapImage.dx
                y: mapImage.dy
            }
        ]

        width: canvas.width
        height: canvas.height

        /*Rectangle {
            color: "red"
            opacity: 0.5
            anchors.fill: mapImage
        }*/
    }


    PinchArea {
        id: pinchArea
        anchors.fill: canvas

        // initial values
        property bool pinching
        property real startDx
        property real startDy
        property real startScale

        onPinchStarted: {
            pinching = true
            startDx = mapImage.dx
            startDy = mapImage.dy
            startScale = mapImage.scale
        }
        onPinchUpdated: {
            var ds = pinch.scale/pinch.previousScale
            mapImage.scale = startScale * pinch.scale
            mapImage.dx = startDx + ( pinch.center.x - pinch.startCenter.x ) * pinch.scale
            mapImage.dy = startDy + ( pinch.center.y - pinch.startCenter.y ) * pinch.scale
        }
        onPinchFinished: {
            mapEngine.move(pinch.startCenter.x, pinch.startCenter.y, pinch.center.x, pinch.center.y)
            mapEngine.scale(pinch.scale)
            mapImage.setPrevious()
            pinching = false
        }

    // mouse area needs to be inside pinch area in order to have
    // both pinch and mouse area working together
    MouseArea {
        id: mouseArea
        anchors.fill: pinchArea

        property bool panning: false
        property point panStart
        property real dxStart
        property real dyStart

        property bool zooming: false
        property real wheelScaleStart

        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                panning = true
                panStart = Qt.point( mouse.x, mouse.y )
                dxStart = mapImage.dx
                dyStart = mapImage.dy
            }
        }

        onReleased: {
            if (mouse.button == Qt.LeftButton) {
                panning = false
                if (Math.abs(panStart.x - mouse.x) == 0 && Math.abs(panStart.y - mouse.y) == 0)
                    canvas.clicked(mouse.x, mouse.y)
                else
                {
                    mapEngine.move(panStart.x, panStart.y, mouse.x, mouse.y)
                    mapImage.setPrevious()
                }

            }
        }

        onPositionChanged: {
            if (panning) {
                mapImage.dx = mouse.x - panStart.x + dxStart
                mapImage.dy = mouse.y - panStart.y + dyStart
            }
        }

        onPressAndHold: canvas.pressAndHold(mouse.x, mouse.y)

        function temporaryZoom(d) {
            if (!zooming)
            {
                zooming = true
                wheelScaleStart = mapImage.scale
            }
            mapImage.scale *= d
            mapImage.dx *= d
            mapImage.dy *= d

            zoomRefreshTimer.restart()
        }

        Timer {
             id: zoomRefreshTimer
             interval: 100
             onTriggered: {
                 mapEngine.scale(mapImage.scale / mouseArea.wheelScaleStart)
                 mapImage.setPrevious()
                 mouseArea.zooming = false
             }
         }

        onWheel: {
            if (wheel.angleDelta.y > 0)
                temporaryZoom(1.1)
            else if (wheel.angleDelta.y < 0)
                temporaryZoom(0.9)
        }

        onDoubleClicked: {
            if (mouse.button == Qt.RightButton)
                temporaryZoom(1.1)
            else
                temporaryZoom(0.9)
        }

    }

    }

}
