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
import "./components" as MMComponents
import "."  // import InputStyle singleton

Rectangle {
  id: root
  height: InputStyle.rowHeight
  width: parent.width
  color: InputStyle.clrPanelMain

  property string text: ""
  property bool isNew: false

  RowLayout {
    anchors.fill: parent
    anchors.leftMargin: InputStyle.panelMargin
    anchors.rightMargin: InputStyle.panelMargin
    anchors.topMargin: InputStyle.innerFieldMargin
    anchors.bottomMargin: InputStyle.innerFieldMargin

    spacing: InputStyle.panelSpacing

    Text {
      Layout.fillWidth: true
      Layout.preferredWidth: root.width * 0.5

      text: root.text

      horizontalAlignment: Text.AlignLeft
      verticalAlignment: Text.AlignVCenter

      elide: Text.ElideRight

      color: InputStyle.fontColor
      font.bold: true
      font.pixelSize: InputStyle.fontPixelSizeNormal
    }

    MMComponents.DelegateButton {
      Layout.preferredWidth: root.width * 0.1

      btnWidth: width
      btnHeight: InputStyle.mediumBtnHeight
      height: InputStyle.rowHeightMedium

      text: qsTr("New!")

      fontPixelSize: InputStyle.fontPixelSizeSmall
      bgColor: InputStyle.fontColor

      visible: root.isNew
    }

    Rectangle {
      Layout.fillWidth: true
      Layout.preferredWidth: root.width * 0.5
      color: "transparent"
    }
  }
}
