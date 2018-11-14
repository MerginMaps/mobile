import QtQuick 2.7
import QtQuick.Controls 2.2
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Drawer {

    property var mapSettings
    property var project
    property real panelHeight
    property real previewHeight

    property alias formState: featureForm.state
    property alias feature: attributeModel.featureLayerPair
    property alias currentAttributeModel: attributeModel


    id: featurePanel
    visible: false
    modal: false
    interactive: true
    dragMargin: 0 // prevents opening the drawer by dragging.
    edge: Qt.BottomEdge

    background: Rectangle {
        id: stateManager
        color: InputStyle.clrPanelBackground

        state: "preview"
        states: [
            State {
                name: "preview"
                PropertyChanges { target: featurePanel; height: featurePanel.previewHeight }
                PropertyChanges { target: featureForm; visible: false }
                PropertyChanges { target: previewPanel; visible: true }
            }
            ,State {
                name: "form"
                PropertyChanges { target: featurePanel; height: featurePanel.panelHeight }
                PropertyChanges { target: featureForm; visible: true }
                PropertyChanges { target: previewPanel; visible: false }
            }
        ]

    }

    onClosed: {
        stateManager.state = "preview"
    }

    function show_panel(feature, formState) {
        featurePanel.feature = feature
        featurePanel.formState = formState
        featurePanel.visible = true
        previewPanel.visible = true
        var index = currentAttributeModel.index(0, 0)
        previewPanel.title = currentAttributeModel.data(index, QgsQuick.AttributeFormModel.Name)
    }


    PreviewPanel {
      id: previewPanel
      height: featurePanel.previewHeight
      width: parent.width
      visible: false
      x: 0
      y: 0
      onContentClicked: {
          stateManager.state = "form"
          console.log("onContentClicked",stateManager.state)
      }
    }

    QgsQuick.FeatureForm {
      id: featureForm

      // using anchors here is not working well as
      width: featurePanel.width
      height: featurePanel.height

      model: QgsQuick.AttributeFormModel {
        attributeModel: QgsQuick.AttributeModel {
            id: attributeModel
        }
      }

      project: featurePanel.project

      toolbarVisible: true

      onSaved: {
        featurePanel.visible = false
      }
      onCanceled: featurePanel.visible = false
    }

}
