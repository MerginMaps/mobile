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

import lc 1.0
import ".."
import "../components"

Item {
  id: root

  property var project
  property var featureLayerPair

  property var linkedRelation
  property var parentController

  property string formState

  signal close()
  signal editGeometryClicked()
  signal openLinkedFeature( var linkedFeature )
  signal createLinkedFeature( var parentController, var relation )

  function updateFeatureGeometry() {
    let f = formStackView.get( 0 )

    if ( f ) {
      f.form.controller.save()
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

    initialItem: formPageComponent
    focus: true

    onCurrentItemChanged: {
      currentItem.forceActiveFocus()
    }
  }

  Component {
    id: formPageComponent

    Page {
      id: formPage

      property alias form: featureForm

      header: PanelHeader {
        id: header


        height: InputStyle.rowHeightHeader
        rowHeight: InputStyle.rowHeightHeader
        color: InputStyle.clrPanelMain
        fontBtnColor: InputStyle.highlightColor

        titleText: featureForm.state === "edit" ? qsTr("Edit Feature") : qsTr("Feature")

        backIconVisible: !saveButtonText.visible
        backTextVisible: saveButtonText.visible

        onBack: featureForm.cancel()

        Text {
          id: saveButtonText

          text: qsTr("Save")

          height: header.rowHeight
          visible: featureForm.state === "edit" || featureForm.state === "add"

          color: featureForm.controller.hasValidationErrors ? InputStyle.invalidButtonColor : InputStyle.highlightColor
          font.pixelSize: InputStyle.fontPixelSizeNormal

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

      Item {
        id: backHandler
        focus: true
        Keys.onReleased: {
          if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
            if ( featureForm.controller.hasAnyChanges )  {
              saveChangesDialog.open()
            }
            else {
              root.close()
            }
            event.accepted = true;
          }
        }

        onVisibleChanged: {
          if ( visible )
            backHandler.forceActiveFocus()
        }
      }

      // content
      FeatureForm {
        id: featureForm

        anchors.fill: parent

        project: root.project

        controller: AttributeController {
          featureLayerPair: root.featureLayerPair
          variablesManager: __variablesManager

          rememberAttributesController: RememberAttributesController {
            rememberValuesAllowed: __appSettings.reuseLastEnteredValues
          }
        }

        importDataHandler: codeReaderHandler.handler
        externalResourceHandler: externalResourceBundle.handler
        state: root.formState

        onSaved: root.close()
        onCanceled: root.close()
        onOpenLinkedFeature: root.openLinkedFeature( linkedFeature )
        onCreateLinkedFeature: root.createLinkedFeature( parentController, relation )

        extraView: formPage.StackView.view

        Connections {
          target: root
          onFormStateChanged: featureForm.state = root.formState
        }

        Component.onCompleted: {
          if ( root.parentController && root.linkedRelation ) {
            featureForm.controller.parentController = root.parentController
            featureForm.controller.linkedRelation = root.linkedRelation
          }
        }
      }

      footer: FeatureToolbar {
        id: toolbar

        height: InputStyle.rowHeightHeader

        state: featureForm.state

        visible: !root.readOnly
        isFeaturePoint: __inputUtils.geometryFromLayer( root.featureLayerPair.layer ) === "point"

        onEditClicked: root.formState = "edit"
        onDeleteClicked: deleteDialog.visible = true
        onEditGeometryClicked: root.editGeometryClicked()
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
          if ( clickedButton === StandardButton.Ok ) {
            featureForm.controller.deleteFeature()
            featureForm.canceled()
            root.close()
          }

          visible = false
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

      ExternalResourceBundle {
        id: externalResourceBundle
      }

      CodeReaderHandler {
        id: codeReaderHandler
      }
    }
  }
}
