/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "../"
import "../components"

Item {
  id: root
  property var featureLayerPair
  property string formState
  property var linkedRelation
  property var parentController
  property var project

  signal close
  signal createLinkedFeature(var parentController, var relation)
  signal editGeometryClicked
  signal openLinkedFeature(var linkedFeature)
  function updateFeatureGeometry() {
    let f = formStackView.get(0);
    if (f) {
      f.form.controller.save();
    }
  }

  StackView {
    id: formStackView

    /**
     * StackView handling navigation in one FeatureForm
     * Initial page is the form itself and any other extra
     * needed pages (like value relation page, relations page, ..)
     * should be pushed to this view.
     *
     * View is attached to Feature Form,
     * so editors can push their components to it
     */
    anchors.fill: parent
    focus: true
    initialItem: formPageComponent

    onCurrentItemChanged: {
      currentItem.forceActiveFocus();
    }
  }
  Component {
    id: formPageComponent
    Page {
      id: formPage
      property alias form: featureForm

      Item {
        id: backHandler
        focus: true

        Keys.onReleased: {
          if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
            if (featureForm.controller.hasAnyChanges) {
              saveChangesDialog.open();
            } else {
              root.close();
            }
            event.accepted = true;
          }
        }
        onVisibleChanged: {
          if (visible)
            backHandler.forceActiveFocus();
        }
      }

      // content
      FeatureForm {
        id: featureForm
        anchors.fill: parent
        externalResourceHandler: externalResourceBundle.handler
        extraView: formPage.StackView.view
        importDataHandler: codeReaderHandler.handler
        project: root.project
        state: root.formState

        Component.onCompleted: {
          if (root.parentController && root.linkedRelation) {
            featureForm.controller.parentController = root.parentController;
            featureForm.controller.linkedRelation = root.linkedRelation;
          }
        }
        onCanceled: root.close()
        onCreateLinkedFeature: root.createLinkedFeature(parentController, relation)
        onOpenLinkedFeature: root.openLinkedFeature(linkedFeature)
        onSaved: root.close()

        Connections {
          target: root

          onFormStateChanged: featureForm.state = root.formState
        }

        controller: AttributeController {
          featureLayerPair: root.featureLayerPair
          variablesManager: __variablesManager

          rememberAttributesController: RememberAttributesController {
            rememberValuesAllowed: __appSettings.reuseLastEnteredValues
          }
        }
      }
      MessageDialog {
        id: deleteDialog
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        text: qsTr("Are you sure you want to delete this feature?")
        title: qsTr("Delete feature")
        visible: false

        //! Using onButtonClicked instead of onAccepted,onRejected which have been called twice
        onButtonClicked: {
          if (clickedButton === StandardButton.Ok) {
            featureForm.controller.deleteFeature();
            featureForm.canceled();
            root.close();
          }
          visible = false;
        }
      }
      MessageDialog {
        id: saveChangesDialog
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Yes | StandardButton.No | StandardButton.Cancel
        text: qsTr("Do you want to save changes?")
        title: qsTr("Unsaved changes")
        visible: false

        //! Using onButtonClicked instead of onAccepted,onRejected which have been called twice
        onButtonClicked: {
          if (clickedButton === StandardButton.Yes) {
            featureForm.save();
          } else if (clickedButton === StandardButton.No) {
            featureForm.canceled();
          } else if (clickedButton === StandardButton.Cancel)
          // Do nothing
          {
          }
          visible = false;
        }
      }
      ExternalResourceBundle {
        id: externalResourceBundle
      }
      CodeReaderHandler {
        id: codeReaderHandler
      }

      footer: FeatureToolbar {
        id: toolbar
        height: InputStyle.rowHeightHeader
        isFeaturePoint: __inputUtils.geometryFromLayer(root.featureLayerPair.layer) === "point"
        state: featureForm.state
        visible: !root.readOnly

        onDeleteClicked: deleteDialog.visible = true
        onEditClicked: root.formState = "edit"
        onEditGeometryClicked: root.editGeometryClicked()
      }
      header: PanelHeader {
        id: header
        backIconVisible: !saveButtonText.visible
        backTextVisible: saveButtonText.visible
        color: InputStyle.clrPanelMain
        fontBtnColor: InputStyle.highlightColor
        height: InputStyle.rowHeightHeader
        rowHeight: InputStyle.rowHeightHeader
        titleText: featureForm.state === "edit" ? qsTr("Edit Feature") : qsTr("Feature")

        onBack: featureForm.cancel()

        Text {
          id: saveButtonText
          anchors.bottom: parent.bottom
          anchors.right: parent.right
          anchors.rightMargin: InputStyle.panelMargin // same as back button
          anchors.top: parent.top
          color: featureForm.controller.hasValidationErrors ? InputStyle.invalidButtonColor : InputStyle.highlightColor
          font.pixelSize: InputStyle.fontPixelSizeNormal
          height: header.rowHeight
          horizontalAlignment: Text.AlignLeft
          text: qsTr("Save")
          verticalAlignment: Text.AlignVCenter
          visible: featureForm.state === "edit" || featureForm.state === "add"

          MouseArea {
            anchors.fill: parent

            onClicked: featureForm.save()
          }
        }
      }
    }
  }
}
