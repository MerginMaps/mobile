/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2

import "."  // import InputStyle singleton
import QgsQuick 0.1 as QgsQuick
import lc 1.0

Drawer {

    property var mapSettings
    property var project
    property real panelHeight
    property real previewHeight
    property bool isReadOnly

    signal editGeometryClicked()
    signal panelClosed()

    property alias formState: featureForm.state
    property alias feature: attributeController.featureLayerPair

    function saveFeatureGeom() {
        featureForm.save()
    }

    function isNewFeature() {
      return attributeController.isNewFeature()
    }

    function reload() {
      attributeController.reset()
      featureForm.reset()
      rememberAttributesController.reset()
      attributePreviewController.reset()
    }

    function onAboutToClose() {
      if (attributeController.hasAnyChanges)  {
        saveChangesDialog.open()
      } else {
        featurePanel.visible = false
      }
    }

    id: featurePanel
    visible: false
    modal: false
    interactive: previewPanel.visible
    dragMargin: 0 // prevents opening the drawer by dragging.
    edge: Qt.BottomEdge
    closePolicy: Popup.CloseOnEscape // prevents the drawer closing while moving canvas

    Behavior on height {
        PropertyAnimation { properties: "height"; easing.type: Easing.InOutQuad }
    }

    RememberAttributesController {
      id: rememberAttributesController
      rememberValuesAllowed: __appSettings.reuseLastEnteredValues
    }

    AttributeController {
      id: attributeController
      rememberAttributesController: rememberAttributesController
      variablesManager: __variablesManager
    }

    AttributePreviewController {
      id: attributePreviewController
      project: __loader.project
    }

    Item {
      id: backHandler
      focus: true
      Keys.onReleased: {
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
          featurePanel.onAboutToClose()
        }
      }
    }

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
        attributePreviewController.featureLayerPair = feature
        featurePanel.formState = formState
        featurePanel.visible = true
        featurePanel.isReadOnly = feature.layer.readOnly
        backHandler.focus = true
        stateManager.state = panelState
    }

    PreviewPanel {
      id: previewPanel
      controller: attributePreviewController
      height: featurePanel.previewHeight
      width: parent.width
      visible: false
      isReadOnly: featurePanel.isReadOnly
      x: 0
      y: 0
      onContentClicked: {
          stateManager.state = "form"
      }

      onEditClicked: {
          stateManager.state = "form"
          featurePanel.formState = "Edit"
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
            fontBtnColor: InputStyle.highlightColor
            rowHeight: InputStyle.rowHeightHeader
            titleText: featurePanel.formState === "Edit" ? qsTr("Edit Feature") : qsTr("Feature")
            backIconVisible: !saveButtonText.visible
            backTextVisible: saveButtonText.visible

            onBack: {
              featurePanel.close()
            }

            Text {
                id: saveButtonText
                text: qsTr("Save")
                visible: featureForm.state === "Edit" || featureForm.state === "Add"
                enabled: featureForm.controller.fieldValuesValid && featureForm.controller.constraintsHardValid
                color: enabled ? InputStyle.highlightColor : "red"
                font.pixelSize: InputStyle.fontPixelSizeNormal
                height: header.rowHeight
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.top: parent.top
                anchors.rightMargin: InputStyle.panelMargin // same as back button

                MouseArea {
                  anchors.fill: parent
                  onClicked: featureForm.save()
                }
            }

        }

        FeatureForm {
            id: featureForm
            visible: true

            width: parent.width
            height: parent.height - header.height - toolbar.height
            anchors.top: header.bottom
            anchors.bottom: toolbar.top
            externalResourceHandler: externalResourceBundle.handler
            importDataHandler: codeReaderHandler.handler
            controller: attributeController
            project: featurePanel.project


            onSaved: {
                featurePanel.panelClosed()
                featurePanel.visible = false
            }
            onCanceled: {
              featurePanel.panelClosed()
              featurePanel.visible = false
            }

            onStateChanged: {
                toolbar.state = featureForm.state
            }

            onNotify: __inputUtils.showNotificationRequested(message)

            customWidgetCallback: valueRelationWidget.handler
        }

        FeatureToolbar {
            id: toolbar
            width: parent.width
            height: InputStyle.rowHeightHeader
            y: parent.height - height
            state: featurePanel.formState
            visible: !featurePanel.isReadOnly
            isFeaturePoint: featurePanel.feature.layer && digitizing.hasPointGeometry(featurePanel.feature.layer)

            onEditClicked: featureForm.state = "Edit"
            onDeleteClicked: deleteDialog.visible = true
            onEditGeometryClicked: {
                featurePanel.editGeometryClicked()
            }
        }


        MessageDialog {
          id: deleteDialog
          visible: false
          title: qsTr( "Delete feature" )
          text: qsTr( "Are you sure you want to delete this feature?" )
          icon: StandardIcon.Warning
          standardButtons: StandardButton.Ok | StandardButton.Cancel

          //! Using onButtonClicked instead of onAccepted,onRejected which have been called twice
          onButtonClicked: {
              if (clickedButton === StandardButton.Ok) {
                attributeController.deleteFeature()
                visible = false
                featureForm.canceled()
              }
              else if (clickedButton === StandardButton.Cancel) {
                visible = false
              }
          }
        }

        MessageDialog {
          id: saveChangesDialog
          visible: false
          title: qsTr( "Unsaved changes" )
          text: qsTr( "Do you want to save changes?" )
          icon: StandardIcon.Warning
          standardButtons: StandardButton.Yes | StandardButton.No | StandardButton.Cancel

          //! Using onButtonClicked instead of onAccepted,onRejected which have been called twice
          onButtonClicked: {
              if (clickedButton === StandardButton.Yes) {
                featureForm.save()
              }
              else if (clickedButton === StandardButton.No) {
                featureForm.canceled()
              }
              else if (clickedButton === StandardButton.Cancel) {
                // Do nothing
              }
              visible = false
          }
        }
    }

    ExternalResourceBundle {
      id: externalResourceBundle
    }

    ValueRelationWidget {
      id: valueRelationWidget

      onWidgetClosed: backHandler.forceActiveFocus()
    }

    CodeReaderHandler {
      id: codeReaderHandler
    }

}
