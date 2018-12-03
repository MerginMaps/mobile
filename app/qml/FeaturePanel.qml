import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
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
    closePolicy: Popup.CloseOnEscape // prevents the drawer closing while moving canvas

    background: Rectangle {
        id: stateManager
        color: InputStyle.clrPanelMain

        state: "preview"
        states: [
            State {
                name: "preview"
                PropertyChanges { target: featurePanel; height: featurePanel.previewHeight }
                PropertyChanges { target: formContainer; visible: false }
                PropertyChanges { target: previewPanel; visible: true }
            }
            ,State {
                name: "form"
                PropertyChanges { target: featurePanel; height: featurePanel.panelHeight }
                PropertyChanges { target: formContainer; visible: true }
                PropertyChanges { target: previewPanel; visible: false }
            }
        ]

    }

    onClosed: {
        stateManager.state = "preview"
    }

    function show_panel(feature, formState, panelState) {
        featurePanel.feature = feature
        featurePanel.formState = formState
        featurePanel.visible = true

        if (panelState === "preview") {
            var index = currentAttributeModel.index(0, 0)
            previewPanel.title = currentAttributeModel.data(index, QgsQuick.AttributeFormModel.Name)
            previewPanel.previewFields = __loader.mapTip(feature)
        }
        stateManager.state = panelState
    }


    PreviewPanel {
      id: previewPanel
      model: featureForm.model
      height: featurePanel.previewHeight
      width: parent.width
      visible: false
      x: 0
      y: 0
      onContentClicked: {
          stateManager.state = "form"
      }
    }


    Item {
        id: formContainer
        width: featurePanel.width
        height: featurePanel.height
        visible: false

        PanelHeader {
            id: header
            height: InputStyle.rowHeightHeader
            width: parent.width
            color: InputStyle.clrPanelMain
            rowHeight: InputStyle.rowHeightHeader
            titleText: "Edit Object"

            onBack: featurePanel.visible = false
        }

        // TODO currently disabled since supporting photos is not yet implemented
        Rectangle {
            id: photoContainer
            height: 0
            visible: false
            width: parent.width
            anchors.top: header.bottom
            color: InputStyle.panelBackground2

            Text {
                id: backButtonText
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "No photos added."
                color: InputStyle.clrPanelMain
                font.pixelSize: InputStyle.fontPixelSizeNormal
            }
        }

        QgsQuick.FeatureForm {
            id: featureForm
            visible: true

            width: parent.width
            height: parent.height - header.height - photoContainer.height - toolbar.height
            anchors.top: photoContainer.bottom
            anchors.bottom: toolbar.top

            model: QgsQuick.AttributeFormModel {
                attributeModel: QgsQuick.AttributeModel {
                    id: attributeModel
                }
            }

            project: featurePanel.project
            onSaved: {
                featurePanel.visible = false
            }
            onCanceled: featurePanel.visible = false

            onStateChanged: {
                toolbar.state = featureForm.state
            }
        }

        FeatureToolbar {
            id: toolbar
            width: parent.width
            height: InputStyle.rowHeightHeader
            y: parent.height - height
            state: featurePanel.formState

            onEditClicked: featureForm.state = "Edit"
            onSaveClicked: featureForm.save()
            onDeleteClicked: deleteDialog.visible = true
        }


        MessageDialog {
          id: deleteDialog
          visible: false
          title: qsTr( "Delete feature" )
          text: qsTr( "Really delete this feature?" )
          icon: StandardIcon.Warning
          standardButtons: StandardButton.Ok | StandardButton.Cancel
          onAccepted: {
            featureForm.model.attributeModel.deleteFeature()
            visible = false
            featureForm.canceled()
          }
          onRejected: {
            visible = false
          }
        }
    }
}
