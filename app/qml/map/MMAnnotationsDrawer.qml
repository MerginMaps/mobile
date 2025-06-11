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

  property MM.AnnotationsController annotationsController

  Component.onCompleted: root.open()

  modal: false
  interactive: false
  closePolicy: Popup.CloseOnEscape

  dropShadow: true

  Behavior on implicitHeight {
    PropertyAnimation { properties: "implicitHeight"; easing.type: Easing.InOutQuad }
  }

  drawerHeader.title: qsTr( "Annotations" )

  drawerHeader.topLeftItemContent: MMButton {
    type: MMButton.Types.Primary
    text: qsTr( "Undo" )
    iconSourceLeft: __style.undoIcon
    bgndColor: __style.lightGreenColor
    size: MMButton.Sizes.Small
    enabled: root.annotationsController?.canUndo ?? false

    anchors {
      left: parent.left
      leftMargin: __style.pageMargins + __style.safeAreaLeft
      verticalCenter: parent.verticalCenter
    }

    onClicked: root.annotationsController.undo()
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

          model: root.annotationsController?.availableColors() ?? null

          Button {
            implicitWidth: 42
            implicitHeight: 42
            background: Rectangle {
              color: modelData
              radius: __style.radius30
              border.width: 1 + ( root.annotationsController.activeColor.toString().toLowerCase() === modelData.toLowerCase() ? 2 : 0 )
              border.color: "black"
            }

            onClicked: {
              root.annotationsController.eraserActive = false
              root.annotationsController.activeColor = modelData
            }

            Component.onCompleted: {
              // set the initial color to be the first one in the list
              if ( index === 0 )
              {
                root.annotationsController.activeColor = modelData
              }
            }
          }
        }

        MMButton {
          text: qsTr( "Eraser" )
          iconSourceLeft: __style.editIcon

          type: MMButton.Types.Primary
          size: MMButton.Sizes.Small

          fontColor: root.annotationsController?.eraserActive ? __style.negativeColor : __style.grapeColor
          iconColor: root.annotationsController?.eraserActive ? __style.negativeColor : __style.grapeColor
          bgndColor: root.annotationsController?.eraserActive ? __style.grapeColor : __style.negativeColor
          fontColorHover: root.annotationsController?.eraserActive ? __style.grapeColor : __style.negativeColor
          iconColorHover: root.annotationsController?.eraserActive ? __style.grapeColor : __style.negativeColor
          bgndColorHover: root.annotationsController?.eraserActive ? __style.negativeColor : __style.grapeColor

          onClicked: {
            root.annotationsController.activeColor = null
            root.annotationsController.eraserActive = true
          }
        }
      }
    }
  }
}
