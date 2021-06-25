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

import ".."
import lc 1.0
import QgsQuick 0.1 as QgsQuick

Item {
  id: root

  property var project
  property var featureLayerPair
  property var parentFeatureLayerPair
  property bool readOnly

  signal openNewForm( var i, var myParent )
  signal close()
  signal editGeometryClicked()

  function openItem( item ) {
    formStackView.push( item )
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
     * so fields can push their components to it
     */

    anchors.fill: parent

    initialItem: formPageComponent
  }

  Component {
    id: formPageComponent

    Page {
      id: formPage

      anchors.fill: parent

      header: PanelHeader {
        id: header

        height: InputStyle.rowHeightHeader
        rowHeight: InputStyle.rowHeightHeader
//        width: parent.width
        color: InputStyle.clrPanelMain
        fontBtnColor: InputStyle.highlightColor

        titleText: featurePanel.formState === "Edit" ? qsTr("Edit Feature") : qsTr("Feature")

        backIconVisible: !saveButtonText.visible
        backTextVisible: saveButtonText.visible

        onBack: root.close()
//        {
//          featurePanel.close()
//        }

        Text {
          id: saveButtonText

          text: qsTr("Save")

          height: header.rowHeight
          visible: featureForm.state === "Edit" || featureForm.state === "Add"

          enabled: featureForm.controller.fieldValuesValid && featureForm.controller.constraintsHardValid

          color: enabled ? InputStyle.highlightColor : "red"
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

      // content
      FeatureForm {
        id: featureForm

        anchors.fill: parent

//        anchors {
//          left: parent.right
//          right: parent.right
//        }

//        width: parent.width
//        height: parent.height - header.height - toolbar.height

//        anchors.top: header.bottom
//        anchors.bottom: toolbar.top


        project: root.project
        controller: attributeController

        importDataHandler: codeReaderHandler.handler
        externalResourceHandler: externalResourceBundle.handler

        onSaved: root.close()
//        {
//          featurePanel.panelClosed()
//          featurePanel.visible = false
//        }

        onCanceled: root.close()
//        {
//          featurePanel.panelClosed()
//          featurePanel.visible = false
//        }

//        onStateChanged: {
//          toolbar.state = featureForm.state
//        }

        customWidgetCallback: valueRelationWidget.handler
      }

      footer: FeatureToolbar {
        id: toolbar

//        width: parent.width
        height: InputStyle.rowHeightHeader
//        y: parent.height - height

        state: featurePanel.formState

        visible: !root.readOnly
        isFeaturePoint: digitizing.hasPointGeometry( root.featureLayerPair.layer )
//        isFeaturePoint: featurePanel.feature.layer && digitizing.hasPointGeometry(featurePanel.feature.layer)

        onEditClicked: featureForm.state = "Edit"
        onDeleteClicked: deleteDialog.visible = true
        onEditGeometryClicked: {
//          featurePanel.editGeometryClicked()
          root.editGeometryClicked()
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
          if ( clickedButton === StandardButton.Ok ) {
            featureForm.delete()
//            visible = false
//            featureForm.canceled()
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

      ValueRelationWidget {
        id: valueRelationWidget

        onWidgetClosed: featureForm.forceActiveFocus()
      }

      CodeReaderHandler {
        id: codeReaderHandler
      }
    }
  }
}
