import QtQuick 2.0
import QtQuick.Window 2.2
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

    property MapEngine engine: MapEngine { view: canvas.view }

    // emitted when a single point is clicked
    signal clicked(real x, real y)

    // emitted when a single point is clicked for a longer time
    signal pressAndHold(real x, real y)

    color: "white"


    MapImage {
        id: mapImage

        mapEngine: canvas.engine

        property MapView view: MapView { parentView: canvas.view }

        property MapView viewRequest: MapView { }

        onMapImageChanged: {
            console.log("map image changed: resetting center + mupp")
            mapImage.view.copyFrom(mapImage.viewRequest)
        }

        function refreshMapImage() {
            viewRequest.copyFrom(canvas.view)
            console.log("refreshMapImage " + viewRequest.center + " + " + viewRequest.mupp + " + " + viewRequest.size + " = " + viewRequest.toExtent())
            mapImage.refreshMap(viewRequest)
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

        function isDragging(x,y) {
            // leave some tolerance before we consider that user is dragging the map
            return Math.abs(panStart.x - x) > Screen.pixelDensity*1 || Math.abs(panStart.y - y) > Screen.pixelDensity*1
        }

        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                panStart = Qt.point( mouse.x, mouse.y )
                centerStart = view.center
            }
        }

        onReleased: {
            if (mouse.button == Qt.LeftButton) {
                if (!panning)
                    canvas.clicked(mouse.x, mouse.y)
                else
                {
                    panning = false
                    mapImage.refreshMapImage()
                }

            }
        }

        onPositionChanged: {
            if (!panning && (mouse.buttons & Qt.LeftButton) && isDragging(mouse.x, mouse.y))
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
