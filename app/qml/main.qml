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

    function isPositionOutOfExtent(border) {
        return ((positionKit.screenPosition.x < border) ||
                (positionKit.screenPosition.y < border) ||
                (positionKit.screenPosition.x > mapCanvas.width -  border) ||
                (positionKit.screenPosition.y > mapCanvas.height -  border)
                )
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
                highlight.featureLayerPair = pair
                featurePanel.show_panel(pair, "Add")
            }
            else {
                digitizing.startRecording()
            }
        }
        else {
            // assuming layer with point geometry
            var pair = digitizing.pointFeature()
            highlight.featureLayerPair = pair
            featurePanel.show_panel(pair, "Add")
        }
    }

    Component.onCompleted: {
        openProjectPanel.activeProjectIndex = 0;
        openProjectPanel.visible = true
        InputStyle.deviceRatio = window.screen.devicePixelRatio
        console.log("Completed Running!")
    }

    QgsQuick.MapCanvas {
      id: mapCanvas

      height: parent.height
      width: parent.width

      mapSettings.layers: __layersModel.layers
      mapSettings.project: __loader.project

      QgsQuick.IdentifyKit {
        id: identifyKit
        mapSettings: mapCanvas.mapSettings
        identifyMode: QgsQuick.IdentifyKit.TopDownStopAtFirst
      }

      onClicked: {
        mapCanvas.forceActiveFocus()
        var screenPoint = Qt.point( mouse.x, mouse.y );

        var res = identifyKit.identifyOne(screenPoint);
        highlight.featureLayerPair = res
        if (res.valid) {
          featurePanel.show_panel(res, res.layer === activeLayerPanel.activeVectorLayer ? "Edit" : "ReadOnly" )
        }
      }
    }

    QgsQuick.FeatureHighlight {
      anchors.fill: mapCanvas
      id: highlight
      color: "red"
      mapSettings: mapCanvas.mapSettings
      z: 1
    }

    Item {
        anchors.fill: mapCanvas
        transform: QgsQuick.MapTransform {
            mapSettings: mapCanvas.mapSettings
        }
        z: 1   // make sure items from here are on top of the Z-order
    }

    SettingsPanel {
      id: settingsPanel
      height: window.height
      //width: QgsQuick.Style.dp * 1000
      edge: Qt.RightEdge
      z: 2   // make sure items from here are on top of the Z-order
    }

    /** Position Kit and Marker */
    QgsQuick.PositionKit {
      id: positionKit
      mapSettings: mapCanvas.mapSettings
      simulatePositionLongLatRad: __use_simulated_position ? [-2.9207148, 51.3624998, 0.05] : []

      onScreenPositionChanged: {
        if (settingsPanel.autoCenterMapChecked) {
          var border = mainPanel.height
          if (isPositionOutOfExtent(border)) {
            mapCanvas.mapSettings.setCenter(positionKit.projectedPosition);
          }
        }
      }
    }

    QgsQuick.PositionMarker {
      id: positionMarker
      positionKit: positionKit
      z: 1
    }

    DigitizingController {
        id: digitizing
        positionKit: positionMarker.positionKit
        layer: activeLayerPanel.activeVectorLayer
    }

    MainPanel {
        id: mainPanel
        width: window.width
        height: InputStyle.scale(115)//115 * QgsQuick.Utils.dp
        z: 2

        activeProjectName: openProjectPanel.activeProjectName
        activeLayerName: activeLayerPanel.activeLayerName
        gpsStatus: ""
        lockOnPosition: settingsPanel.autoCenterMapChecked

        onOpenProjectClicked: openProjectPanel.visible = true
        onOpenLayersClicked: activeLayerPanel.visible = true
        onOpenMapThemesClicked: mapThemesPanel.visible = true
        onMyLocationClicked: mapCanvas.mapSettings.setCenter(positionKit.projectedPosition)
        onMyLocationHold: {
            settingsPanel.autoCenterMapChecked =!settingsPanel.autoCenterMapChecked
            popup.text = "Autocenter mode " + (settingsPanel.autoCenterMapChecked ? "on" : "off")
            popup.open()
        }
        onOpenLogClicked: settingsPanel.visible = true
        onZoomToProject: __loader.zoomToProject(mapCanvas.mapSettings)

        recordButton.recording: digitizing.recording
        onAddFeatureClicked: {
            if (digitizing.recording) {
                recordFeature()
            } else {
                openLayersClicked()
            }
        }
    }

    ScaleBar {
        id: scaleBar
        height: 35 * QgsQuick.Utils.dp
        textWidth: 115 * QgsQuick.Utils.dp
        y: window.height - height
        mapSettings: mapCanvas.mapSettings
        preferredWidth: textWidth * 3
    }

    Rectangle {
        id: gpsAccuracy
        width: scaleBar.textWidth
        height: 35 * QgsQuick.Utils.dp
        x: 0
        y: window.height - 2 * (35 * QgsQuick.Utils.dp)
        color: InputStyle.clrPanelBackground
        opacity: InputStyle.panelOpacity
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
        y: mainPanel.height

        onActiveProjectPathChanged: {
            __loader.load(activeProjectPath);
            activeLayerPanel.activeLayerIndex = 0
        }
    }

    ActiveLayerPanel {
        id: activeLayerPanel
        height: window.height
        width: QgsQuick.Utils.dp * 600
        edge: Qt.LeftEdge

        onLayerSettingChanged: {
            console.log("onLayerSettingChanged")
            recordFeature()
        }
    }

    MapThemePanel {
        id: mapThemesPanel
        height: window.height
        width: QgsQuick.Utils.dp * 600
        edge: Qt.LeftEdge
    }

    Notification {
        id: popup
        text: ""
        anchors.centerIn: parent
        width: 400 * QgsQuick.Utils.dp
        height: 160 * QgsQuick.Utils.dp
    }

    FeaturePanel {
        id: featurePanel
        height: window.height
        panelHeight: window.height
        previewHeight: window.height/3
        width: window.width
        mapSettings: mapCanvas.mapSettings
        project: __loader.project
    }

}
