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
import "./form" as Forms
import QgsQuick 0.1 as QgsQuick
import lc 1.0

Item {
  id: root

  /*
   * FeaturePanel component is responsible for entire feature form, both preview and fullscreen form
   */

  property var project
  property var activeFeatureLayerPair

  property int openedFormsCount: formsStack.depth

  function openForm( pair, pairState, openPreview ) {

    if ( openPreview ) {
      // push to the stackview component with such thing
      formsStack.push()
    }

    // push to the stackview component that does not need preview
    formsStack.push()
  }

  StackView {
    id: formsStack

    anchors.fill: parent

    initialItem: Item{}
  }

  Component {
    id: formComponent // I am featureform and preview

    Drawer {

    //    property var mapSettings
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

        Forms.PreviewPanel {
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
    }
  }
}
