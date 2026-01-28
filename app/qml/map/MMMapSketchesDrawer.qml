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

  drawerHeader.topLeftItemContent: Row{
    width: parent.width - 2 * __style.pageMargins
    spacing: __style.margin12
    anchors{
      left: parent.left
      leftMargin: __style.pageMargins + __style.safeAreaLeft
      verticalCenter:parent.verticalCenter
      rightMargin: __style.pageMargins + __style.safeAreaRight
    }
    
    MMComponents.MMRoundButton {
      iconSource: __style.undoIcon
      bgndColor: __style.lightGreenColor

      enabled: root.sketchingController?.canUndo ?? false

      onClicked: root.sketchingController?.undo()
    }

    MMComponents.MMRoundButton {
      iconSource: __style.redoIcon
      bgndColor: __style.lightGreenColor

      enabled: root.sketchingController?.canRedo ?? false

      onClicked: root.sketchingController?.redo()
    }

    MMComponents.MMRoundButton {
      iconSource: __style.eraserIcon

      bgndColor: root.sketchingController?.eraserActive ? __style.forestColor : __style.lightGreenColor
      iconColor: root.sketchingController?.eraserActive ? __style.grassColor : __style.forestColor

      onClicked: {
        if(root.sketchingController)
        {
          root.sketchingController.eraserActive = true
          root.sketchingController.activeColor = null
        }
      }
    }
  }

  drawerContent: 
    ColumnLayout{

      width: parent.width
      spacing: __style.margin2
      
      MMComponents.MMColorPicker{
        id: colorPicker
        colors: root.sketchingController?.availableColors() ?? __style.photoSketchingWhiteColor

        Layout.alignment: Qt.AlignHCenter
        Layout.maximumWidth: parent.width

        onActiveColorChanged: {
          if(root.sketchingController)
          {
            root.sketchingController.activeColor = colorPicker.activeColor
            root.sketchingController.eraserActive = false
          }
        }
      }
    }
      MMComponents.MMListSpacer{
        height: 2
      }
}