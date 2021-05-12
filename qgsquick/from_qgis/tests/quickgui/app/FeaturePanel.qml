/***************************************************************************
  FeaturePanel.qml
  ----------------
  Date                 : Nov 2017
  Copyright            : (C) 2017 by Peter Petrik
  Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.2
import QgsQuick 0.1 as QgsQuick

import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

Drawer {

  property var mapSettings
  property var project

  property alias state: form.state
  property alias feature: attributeController.featureLayerPair

  /**
   * Icon path for save button.
   */
  property string saveButtonIcon: QgsQuick.Utils.getThemeIcon( "ic_save_white" )
  /**
   * Icon path for delete button.
   */
  property string deleteButtonIcon: QgsQuick.Utils.getThemeIcon( "ic_delete_forever_white" )
  /**
   * Icon path for close button
   */
  property string closeButtonIcon: QgsQuick.Utils.getThemeIcon( "ic_clear_white" )

  id: featurePanel
  visible: false
  modal: true
  interactive: true
  dragMargin: 0 // prevents opening the drawer by dragging.

  background: Rectangle {
    color: "white"
    opacity: 0.5
  }

  function show_panel(feature, state) {
    attributeController.featureLayerPair = feature
    featurePanel.state = state
    featurePanel.visible = true
  }

  QgsQuick.RememberAttributes {
    id: rememberAttributesController
    rememberValuesAllowed: true
  }

  QgsQuick.AttributeController {
    id: attributeController
    rememberAttributes: rememberAttributesController
  }

  QgsQuick.FeatureForm {
    visible: true
    id: form
    // using anchors here is not working well as
    width: featurePanel.width
    height: featurePanel.height - 2 * toolbar.height
    y: 2 * toolbar.height
    controller: attributeController
    project: featurePanel.project
  }

  /** The form toolbar **/
  Item {
    id: toolbar
    height: visible ? 48 * QgsQuick.Utils.dp : 0
    visible: true
    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
    }

    RowLayout {
      anchors.fill: parent
      Layout.margins: 0

      ToolButton {
        id: saveButton
        Layout.preferredWidth: form.style.toolbutton.size
        Layout.preferredHeight: form.style.toolbutton.size

        visible: form.state !== "ReadOnly"

        contentItem: Image {
          source: featurePanel.saveButtonIcon
          sourceSize: Qt.size(width, height)
        }

        background: Rectangle {
          color: attributeController.constraintsSoftValid && attributeController.constraintsHardValid ? form.style.toolbutton.backgroundColor : form.style.toolbutton.backgroundColorInvalid
        }

        enabled: attributeController.constraintsHardValid

        onClicked: {
          form.save()
          featurePanel.visible = false
        }
      }

      ToolButton {
        id: deleteButton

        Layout.preferredWidth: form.style.toolbutton.size
        Layout.preferredHeight: form.style.toolbutton.size

        visible: form.state === "Edit"

        contentItem: Image {
          source: featurePanel.deleteButtonIcon
          sourceSize: Qt.size(width, height)
        }

        background: Rectangle {
          color: form.style.toolbutton.backgroundColor
        }

        onClicked: {
          deleteDialog.visible = true
        }
      }

      Label {
        id: titleLabel

        text:
        {
          var currentLayer = attributeController.featureLayerPair.layer
          var layerName = 'N/A'
          if (!!currentLayer)
            layerName = currentLayer.name

          if ( form.state === 'Add' )
            'Add feature on <i>%1</i>'.arg(layerName )
          else if ( form.state === 'Edit' )
            'Edit feature on <i>%1</i>'.arg(layerName)
          else
            'View feature on <i>%1</i>'.arg(layerName)
        }
        font.bold: true
        font.pointSize:form.style.titleLabelPointSize
        elide: Label.ElideRight
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        Layout.fillWidth: true
        color: "white"
      }

      ToolButton {
        id: closeButton
        Layout.alignment: Qt.AlignRight

        Layout.preferredWidth: form.style.toolbutton.size
        Layout.preferredHeight: form.style.toolbutton.size

        contentItem: Image {
          source: featurePanel.closeButtonIcon
          sourceSize: Qt.size(width, height)
        }

        background: Rectangle {
          color: form.style.toolbutton.backgroundColor
        }

        onClicked: {
          Qt.inputMethod.hide()
          form.canceled()
          featurePanel.visible = false
        }
      }
    }
  }

  MessageDialog {
    id: deleteDialog

    visible: false

    title: "Delete feature"
    text: "Really delete this feature?"
    icon: StandardIcon.Warning
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    onAccepted: {
      attributeController.deleteFeature()
      visible = false

      form.canceled()
      featurePanel.visible = false
    }
    onRejected: {
      visible = false
    }
  }

}

