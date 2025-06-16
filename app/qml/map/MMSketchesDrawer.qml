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

import "../components"
import "./components"

MMDrawer {
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

  drawerHeader.title: qsTr( "Sketches" )

  drawerHeader.topLeftItemContent: MMButton {
    type: MMButton.Types.Primary
    text: qsTr( "Undo" )
    iconSourceLeft: __style.undoIcon
    bgndColor: __style.lightGreenColor
    size: MMButton.Sizes.Small
    enabled: root.sketchingController?.canUndo ?? false

    anchors {
      left: parent.left
      leftMargin: __style.pageMargins + __style.safeAreaLeft
      verticalCenter: parent.verticalCenter
    }

    onClicked: root.sketchingController.undo()
  }

  drawerContent: Column {
    id: mainColumn

    width: parent.width
    spacing: __style.margin10

    ScrollView {
      width: parent.width
      height: scrollRow.height

      ScrollBar.vertical.policy: ScrollBar.AlwaysOff
      ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

      Row {
        id: scrollRow
        width: parent.width
        spacing: __style.margin12

        Repeater {
          id: colorsView

          model: root.sketchingController?.availableColors() ?? null

          Button {
            id: colorButton

            readonly property bool isSelected: root.sketchingController.activeColor.toString().toLowerCase() === modelData.toLowerCase()

            implicitWidth: 40 * __dp
            implicitHeight: 40 * __dp
            background: Rectangle {
              color: modelData
              radius: colorButton.implicitHeight / ( isSelected ? 2.5 : 2 )
              border.width: isSelected ? 4 * __dp : 0
              border.color: "black"
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

        MMButton {
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
