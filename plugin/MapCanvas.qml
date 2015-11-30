import QtQuick 2.0
import qgis 1.0

Rectangle {
    id: canvas

    property MapView view: MapView {
        size: Qt.size(canvas.width, canvas.height)

        onChanged: {
            if (size.width > 0 && size.height > 0 && mupp == 0) {
                console.debug("setting extent - INITIAL")
                this.fromExtent(engine.fullExtent())
            }
            if (!_imgViewInitialized && valid) {
                console.debug("setting image's view - INITIAL")
                _imgViewInitialized = true
                mapImage.refreshMapImage()
            }
        }
    }

    property rect initialExtent
    property bool _imgViewInitialized: false

    property alias engine: mapImage.mapEngine

    // emitted when a single point is clicked
    signal clicked(real x, real y)

    // emitted when a single point is clicked for a longer time
    signal pressAndHold(real x, real y)

    color: "white"


    MapImage {
        id: mapImage

        mapEngine: MapEngine {}

        property MapView view: MapView { parentView: canvas.view }

        property MapView viewRequest: MapView { }

        Connections {
            target: mapImage.mapEngine
            onMapImageChanged: {
                console.log("map image changed: resetting center + mupp")
                mapImage.view.copyFrom(mapImage.viewRequest)
            }
        }

        function refreshMapImage() {
            viewRequest.copyFrom(canvas.view)
            var extent = viewRequest.toExtent()
            console.log("refreshMapImage " + viewRequest.center + " + " + viewRequest.mupp + " + " + viewRequest.size + " = " + extent)
            engine.imageSize = viewRequest.size
            engine.extent = extent
            engine.refreshMap()
        }

        x: mapImage.view.dxToParent
        y: mapImage.view.dyToParent
        width: mapImage.view.scaleToParent * mapImage.view.size.width
        height: mapImage.view.scaleToParent * mapImage.view.size.height
    }


    PinchArea {
        id: pinchArea
        anchors.fill: canvas

        // initial values
        property point startCenter
        property real startMupp

        onPinchStarted: {
            startCenter = canvas.view.center
            startMupp = canvas.view.mupp
        }
        onPinchUpdated: {
            canvas.view.mupp = startMupp / pinch.scale

            var dx = (pinch.center.x - pinch.startCenter.x) * canvas.view.mupp
            var dy = -(pinch.center.y - pinch.startCenter.y) * canvas.view.mupp
            canvas.view.center = Qt.point(startCenter.x - dx, startCenter.y - dy)
        }
        onPinchFinished: {
            mapImage.refreshMapImage()
        }

    // mouse area needs to be inside pinch area in order to have
    // both pinch and mouse area working together
    MouseArea {
        id: mouseArea
        anchors.fill: pinchArea

        property bool panning: false
        property point panStart
        property point centerStart

        property bool zooming: false

        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                panStart = Qt.point( mouse.x, mouse.y )
                centerStart = view.center
            }
        }

        onReleased: {
            if (mouse.button == Qt.LeftButton) {
                panning = false
                if (Math.abs(panStart.x - mouse.x) == 0 && Math.abs(panStart.y - mouse.y) == 0)
                    canvas.clicked(mouse.x, mouse.y)
                else
                {
                    mapImage.refreshMapImage()
                }

            }
        }

        onPositionChanged: {
            if (!panning && (mouse.buttons & Qt.LeftButton))
                panning = true

            if (panning) {
                var dx = (mouse.x - panStart.x) * canvas.view.mupp
                var dy = -(mouse.y - panStart.y) * canvas.view.mupp
                canvas.view.center = Qt.point(centerStart.x - dx, centerStart.y - dy)
            }
        }

        onPressAndHold: if (!panning) canvas.pressAndHold(mouse.x, mouse.y)

        function temporaryZoom(d) {
            if (!zooming)
            {
                zooming = true
            }
            view.mupp /= d

            zoomRefreshTimer.restart()
        }

        Timer {
             id: zoomRefreshTimer
             interval: 100
             onTriggered: {
                 mapImage.refreshMapImage()
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
