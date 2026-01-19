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

    onClicked: root.sketchingController?.undo()
  }

  drawerContent: 
    ColumnLayout{

      width: parent.width
      spacing: __style.margin10
      
      MMComponents.MMColorPicker{
        colors: root.sketchingController?.availableColors() ?? __style.photoSketchingWhiteColor
        showEraseButton: true

        Layout.alignment: Qt.AlignHCenter
        Layout.preferredHeight: scrollRow.height
        Layout.preferredWidth: scrollRow.width
        Layout.maximumWidth: parent.width - ( 2 * __style.pageMargins)

        onActiveColorChanged: {
          root.sketchingController?.activeColor = activeColor
        }

        onEraserActiveChanged: {
          root.sketchingController?.eraserActive = eraserActive
        }
      }
    }
}
