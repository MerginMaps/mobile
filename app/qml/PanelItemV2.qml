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
import "."  // import InputStyle singleton

Rectangle {
    id: root
    height: InputStyle.rowHeight
    width: parent.width
    color: InputStyle.clrPanelMain

    property string text: ""
    property string text2: ""

    RowLayout {
      anchors.fill: parent

      ColumnLayout {
         Layout.preferredWidth: parent.width * 0.70

         Text {
             Layout.fillWidth: true
             Layout.preferredHeight: parent.height / 3
             Layout.leftMargin: InputStyle.panelMargin

             text: root.text
             horizontalAlignment: Text.AlignLeft
             verticalAlignment: Text.AlignVCenter
             color: InputStyle.fontColor
             font.pixelSize: InputStyle.fontPixelSizeNormal
             elide: Text.ElideRight
         }

         Text {
             Layout.fillWidth: true
             Layout.preferredHeight: 2 * parent.height / 3
             Layout.leftMargin: InputStyle.panelMargin

             text: root.text2
             width: 100
             horizontalAlignment: Text.AlignLeft
             verticalAlignment: Text.AlignVCenter
             color: InputStyle.fontColor
             font.pixelSize: InputStyle.fontPixelSizeSmall
             wrapMode: Text.WordWrap
             maximumLineCount: 2
             elide: Text.ElideRight
         }
      }

      Rectangle {
        Layout.fillWidth: true
        Layout.preferredWidth: parent.width * 0.30
        color: "transparent"
      }
    }
}
