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

      // contentHeight: availableHeight
      // contentWidth: scrollRow.width > parent.width ? scrollRow.width : availableWidth

      Row {
        id: scrollRow
        width: parent.width
        spacing: __style.margin12

        Repeater {
          model: root.annotationsController?.availableColors() ?? null

          Button {
            required property string modelData

            implicitWidth: 42
            implicitHeight: 42
            background: Rectangle {
              color: modelData
              radius: __style.radius30
              border.width: 2 * __dp + ( root.annotationsController.activeColor === modelData ? 3 : 0 )
              border.color: "black"
            }

            onClicked: {
              root.annotationsController.eraserActive = false
              root.annotationsController.activeColor = modelData
              console.log( root.annotationsController.activeColor, modelData )
            }
          }
        }

        MMButton {
          text: qsTr( "Erase" )
          iconSourceLeft: __style.smallEditIcon
          size: MMButton.Sizes.Small
          onClicked: {
            root.annotationsController.activeColor = color
            root.annotationsController.eraserActive = true
          }
        }
      }
    }
  }
}
