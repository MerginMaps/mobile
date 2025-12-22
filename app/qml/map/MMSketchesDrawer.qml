/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Shapes

import mm 1.0 as MM

import "../components" as MMComponents
import "./components"

MMComponents.MMDrawer {
  id: root

  readonly property alias panelHeight: root.height

  property MM.MapSketchingController sketchingController

  Component.onCompleted: root.open()

  modal: false
  interactive: false
  closePolicy: Popup.CloseOnEscape

  dropShadow: true

  Behavior on implicitHeight {
    PropertyAnimation { properties: "implicitHeight"; easing.type: Easing.InOutQuad }
  }

  drawerHeader.title: qsTr( "Sketch" )

  drawerHeader.topLeftItemContent: MMComponents.MMRoundButton {
    iconSource: __style.undoIcon
    bgndColor: __style.lightGreenColor

    enabled: root.sketchingController?.canUndo ?? false

    anchors {
      left: parent.left
      leftMargin: __style.pageMargins + __style.safeAreaLeft
      verticalCenter: parent.verticalCenter
    }

    onClicked: root.sketchingController.undo()
  }

  drawerContent: ColumnLayout {
    anchors {
        left: parent.left
        right: parent.right
    }

    ScrollView {
      Layout.fillWidth: true
      Layout.preferredHeight: scrollRow.height
      
      ScrollBar.vertical.policy: ScrollBar.AlwaysOff
      ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

      Row {
        id: scrollRow
        spacing: __style.margin12
        padding: __style.margin4
        anchors.centerIn: parent

        Repeater {
          id: colorsView

          model: root.sketchingController?.availableColors() ?? null

          MMComponents.MMRoundButton {
            anchors.verticalCenter: parent.verticalCenter

            contentItem: Rectangle {
              color: modelData
              radius: width / 2
              anchors.fill: parent
            }

            background: Rectangle {
              property bool isActive: modelData.toLowerCase() === root.sketchingController.activeColor.toString().toLowerCase()

              anchors.verticalCenter: parent.verticalCenter
              anchors.horizontalCenter: parent.horizontalCenter
              radius: width / 2
              width: __style.margin48
              height: __style.margin48
              color: isActive ? __style.transparentColor : __style.lightGreenColor
              border.width: 2
              border.color: isActive ? __style.grassColor : __style.transparentColor
            }

            onClicked: {
              root.sketchingController.eraserActive = false
              root.sketchingController.activeColor = modelData
            }

            Component.onCompleted: {
              // set the initial color to be the first one in the list
              if ( index === 0 )
              {
                root.sketchingController.activeColor = modelData
              }
            }
          }
        }

        MMComponents.MMButton {
          text: qsTr( "Eraser" )
          iconSourceLeft: __style.editIcon

          type: MMButton.Types.Primary
          size: MMButton.Sizes.Small

          fontColor: root.sketchingController?.eraserActive ? __style.negativeColor : __style.grapeColor
          iconColor: root.sketchingController?.eraserActive ? __style.negativeColor : __style.grapeColor
          bgndColor: root.sketchingController?.eraserActive ? __style.grapeColor : __style.negativeColor
          fontColorHover: root.sketchingController?.eraserActive ? __style.grapeColor : __style.negativeColor
          iconColorHover: root.sketchingController?.eraserActive ? __style.grapeColor : __style.negativeColor
          bgndColorHover: root.sketchingController?.eraserActive ? __style.negativeColor : __style.grapeColor

          onClicked: {
            root.sketchingController.activeColor = null
            root.sketchingController.eraserActive = true
          }
        }
      }
    }
  }
}
