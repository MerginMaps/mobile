import QtQuick 2.7
import QtQuick.Controls 2.2
import QgsQuick 0.1 as QgsQuick
import lc 1.0

ApplicationWindow {
    id: window
    visible: true
    width:  __appwindowwidth
    height: __appwindowheight
    visibility: __appwindowvisibility
    title: qsTr("Input")

    property int zMapCanvas: 0
    property int zPanel: 20
    property int zToolkits: 10

    function isPositionOutOfExtent(border) {
        return ((positionKit.screenPosition.x < border) ||
                (positionKit.screenPosition.y < border) ||
                (positionKit.screenPosition.x > mapCanvas.width -  border) ||
                (positionKit.screenPosition.y > mapCanvas.height -  border)
                )
    }

    function saveRecordedFeature(pair) {
        if (pair.valid) {
            digitizingHighlight.featureLayerPair = pair
            digitizingHighlight.visible = true
            featurePanel.show_panel(pair, "Add", "form")
        } else {
            popup.text = "Recording feature is not valid"
            popup.open()
        }
    }

    function recordFeature() {
        var layer = activeLayerPanel.activeVectorLayer
        if (!layer)
        {
            // nothing to do with no active layer
        }
        else if (digitizing.hasLineGeometry(layer)) {
            if (digitizing.recording) {
                digitizing.stopRecording()
                var pair = digitizing.lineFeature()
                saveRecordedFeature(pair)
            }
            else {
                digitizing.startRecording()
            }
        }
        else {
            // assuming layer with point geometry
            var pair = digitizing.pointFeature()
            saveRecordedFeature(pair)
        }
    }

    function getGpsIndicatorColor() {
        if (positionKit.accuracy <= 0) return InputStyle.softRed
        return positionKit.accuracy < __appSettings.gpsAccuracyTolerance ? InputStyle.softGreen : InputStyle.softOrange
    }

    function showMessage(message) {
        if (!__androidUtils.isAndroid) {
            popup.text = message
            popup.open()
        } else {
            __androidUtils.showToast(message)
        }
    }

    Component.onCompleted: {
        if (__appSettings.defaultProject) {
            var path = __appSettings.defaultProject ? __appSettings.defaultProject : openProjectPanel.activeProjectPath
            var defaultIndex = __projectsModel.rowAccordingPath(path)
            var isValid = __projectsModel.data(__projectsModel.index(defaultIndex), ProjectModel.IsValid)
            if (isValid && __loader.load(path)) {
                openProjectPanel.activeProjectIndex = defaultIndex !== -1 ? defaultIndex : 0
                __appSettings.activeProject = path
            } else {
                // if default project load failed, delete default setting
                __appSettings.defaultProject = ""
                openProjectPanel.openPanel()
            }
        } else {
            openProjectPanel.openPanel()
        }

        InputStyle.deviceRatio = window.screen.devicePixelRatio
        InputStyle.realWidth = window.width
        InputStyle.realHeight = window.height
        console.log("Completed Running!")
        __loader.positionKit = positionKit
        __loader.recording = digitizing.recording
    }

    QgsQuick.MapCanvas {
      id: mapCanvas

      height: parent.height - mainPanel.height
      width: parent.width
      z: zMapCanvas

      mapSettings.layers: __layersModel.layers
      mapSettings.project: __loader.project

      QgsQuick.IdentifyKit {
        id: identifyKit
        mapSettings: mapCanvas.mapSettings
        identifyMode: QgsQuick.IdentifyKit.TopDownAll
      }

      onClicked: {
        mapCanvas.forceActiveFocus()
        var screenPoint = Qt.point( mouse.x, mouse.y );
        var res = identifyKit.identifyOne(screenPoint);
        if (res.valid) {
          highlight.featureLayerPair = res
          highlight.visible = true
          featurePanel.show_panel(res, "ReadOnly", "preview" )
        } else if (featurePanel.visible) {
            // closes feature/preview panel when there is nothing to show
            featurePanel.visible = false
        }
      }
    }

    QgsQuick.FeatureHighlight {
      anchors.fill: mapCanvas
      id: highlight
      color: "red"
      mapSettings: mapCanvas.mapSettings
      z: 1
      opacity: digitizing.hasPolygonGeometry(highlight.featureLayerPair.layer) ? InputStyle.lowHighlightOpacity : InputStyle.highHighlightOpacity
    }

    QgsQuick.FeatureHighlight {
      anchors.fill: mapCanvas
      id: digitizingHighlight
      color: "yellow"
      mapSettings: mapCanvas.mapSettings
      z: 1
      opacity: digitizing.hasPolygonGeometry(digitizingHighlight.featureLayerPair.layer) ? InputStyle.lowHighlightOpacity : InputStyle.highHighlightOpacity
    }

    Item {
        anchors.fill: mapCanvas
        transform: QgsQuick.MapTransform {
            mapSettings: mapCanvas.mapSettings
        }
        z: zMapCanvas + 1  // make sure items from here are on top of the Z-order
    }

    SettingsPanel {
      id: settingsPanel
      height: window.height
      width: window.width
      rowHeight: InputStyle.rowHeight
      z: zPanel   // make sure items from here are on top of the Z-order

      onDefaultLayerClicked: activeLayerPanel.openPanel("setup")
      gpsIndicatorColor: getGpsIndicatorColor()
    }

    /** Position Kit and Marker */
    QgsQuick.PositionKit {
      id: positionKit
      mapSettings: mapCanvas.mapSettings
      simulatePositionLongLatRad: __use_simulated_position ? [-2.9207148, 51.3624998, 0.05] : []

      onScreenPositionChanged: {
        if (__appSettings.autoCenterMapChecked) {
          var border = mainPanel.height
          if (isPositionOutOfExtent(border)) {
            mapCanvas.mapSettings.setCenter(positionKit.projectedPosition);
          }
        }
      }
    }

    PositionMarker {
      id: positionMarker
      positionKit: positionKit
      z: zMapCanvas + 2
    }

    DigitizingController {
        id: digitizing
        positionKit: positionMarker.positionKit
        layer: activeLayerPanel.activeVectorLayer

        onRecordingChanged: {
            __loader.recording = digitizing.recording
        }
    }

    // Highlighting a new feature while digitizing
    Connections {
        target: digitizing.recordingFeatureModel
        onFeatureLayerPairChanged: {
            if (digitizing.recording) {
                digitizingHighlight.visible = true
                digitizingHighlight.featureLayerPair = digitizing.recordingFeatureModel.featureLayerPair
            }
        }
    }

    MainPanel {
        id: mainPanel
        width: window.width
        height: InputStyle.rowHeightHeader
        z: zToolkits + 1
        y: window.height - height

        gpsIndicatorColor: getGpsIndicatorColor()

        onOpenProjectClicked: openProjectPanel.openPanel()
        onSetDefaultLayerClicked: activeLayerPanel.openPanel("setup")
        onOpenMapThemesClicked: mapThemesPanel.visible = true
        onMyLocationClicked: mapCanvas.mapSettings.setCenter(positionKit.projectedPosition)
        onMyLocationHold: {
            __appSettings.autoCenterMapChecked =!__appSettings.autoCenterMapChecked
            popup.text = "Autocenter mode " + (__appSettings.autoCenterMapChecked ? "on" : "off")
            popup.open()
        }
        onOpenLogClicked: settingsPanel.visible = true
        onZoomToProject: __loader.zoomToProject(mapCanvas.mapSettings)

        recordButton.recording: digitizing.recording
        onAddFeatureClicked: {
            if (!positionKit.hasPosition) {
                popup.text = qsTr("The GPS is currently not available")
                popup.open()
                return // leaving when no gps is available
            }

            if (digitizing.recording) {
                recordFeature()
            } else {
                activeLayerPanel.openPanel("record")
            }
        }
    }

    ScaleBar {
        id: scaleBar
        height: InputStyle.scaleBarHeight
        y: window.height - height - mainPanel.height - InputStyle.panelMargin
        mapSettings: mapCanvas.mapSettings
        preferredWidth: Math.min(window.width, 180 * QgsQuick.Utils.dp)
        z: zToolkits
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Connections {
        target: mapCanvas.mapSettings
        onExtentChanged: {
            scaleBar.visible = true
        }
    }

    Rectangle {
        id: gpsAccuracy
        width: scaleBar.textWidth
        height: 35 * QgsQuick.Utils.dp
        x: 0
        y: window.height - 2 * (35 * QgsQuick.Utils.dp)
        z: zToolkits
        color: InputStyle.clrPanelBackground
        opacity: InputStyle.panelOpacity
        visible: false
        Text  {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            color: scaleBar.barColor
            font.pixelSize: scaleBar.height - 2 * scaleBar.lineWidth
            text: QgsQuick.Utils.formatDistance( positionKit.accuracy, positionKit.accuracyUnits, 0 )
        }
    }

    ProjectPanel {
        id: openProjectPanel
        height: window.height
        width: window.width
        z: zPanel

        onActiveProjectIndexChanged: {
            openProjectPanel.activeProjectPath = __projectsModel.data(__projectsModel.index(openProjectPanel.activeProjectIndex), ProjectModel.Path)
            __appSettings.activeProject = openProjectPanel.activeProjectPath
            __loader.load(openProjectPanel.activeProjectPath)
            activeLayerPanel.activeLayerIndex = __layersModel.rowAccordingName(__appSettings.defaultLayer)
        }
    }

    ActiveLayerPanel {
        id: activeLayerPanel
        height: window.height/2
        width: window.width
        edge: Qt.BottomEdge
        z: zPanel

        onLayerSettingChanged: {
            recordFeature()
        }
    }

    MapThemePanel {
        id: mapThemesPanel
        height: window.height/2
        width: window.width
        edge: Qt.BottomEdge
        z: zPanel
    }

    Notification {
        id: popup
        text: ""
        width: 400 * QgsQuick.Utils.dp
        height: 160 * QgsQuick.Utils.dp
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        z: zPanel + 1000 // the most top
    }

    Connections {
        target: __merginApi
        onNetworkErrorOccurred: {
            showMessage(message)
        }
        onNotify: {
            showMessage(message)
        }
    }

    FeaturePanel {
        id: featurePanel
        height: window.height
        width: window.width
        mapSettings: mapCanvas.mapSettings
        panelHeight: window.height
        previewHeight: window.height/3
        project: __loader.project
        z: 0 // to featureform editors be visible

        onVisibleChanged: {
            if (!visible) {
                digitizingHighlight.visible = false
                highlight.visible = false
            }
        }
    }
}
