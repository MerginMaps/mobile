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
import "./components"
import "."  // import InputStyle singleton

Rectangle {
    id: root
    height: InputStyle.rowHeight
    width: parent.width
    color: InputStyle.clrPanelMain

    property double value
    property string suffix
    property string text: ""
    property string text2: ""

    signal settingChanged( double value )

    RowLayout {
      anchors.fill: parent
      anchors.leftMargin: InputStyle.panelMargin

      ColumnLayout {
         Layout.preferredWidth: parent.width * 0.70
         Layout.fillHeight: true
         spacing: InputStyle.formSpacing

         Text {
             Layout.fillWidth: true
             Layout.preferredHeight: parent.height / 3

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

             text: root.text2
             horizontalAlignment: Text.AlignLeft
             verticalAlignment: Text.AlignVCenter
             color: InputStyle.secondaryFontColor
             font.pixelSize: InputStyle.fontPixelSizeSmall
             wrapMode: Text.Wrap
             maximumLineCount: 2
             //elide: Text.ElideRight
         }
      }

      NumberInputField {
        id: heightField

        Layout.preferredWidth: parent.width * 0.3
        Layout.fillHeight: true

        number: root.value
        onValueChanged: function(value) {
          root.settingChanged( value )
        }

        suffix: root.suffix

      }
    }

    MouseArea {
      anchors.fill: parent
      onClicked: heightField.getFocus()
    }

}
