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

    color: InputStyle.clrPanelMain

    property double value
    property string suffix
    property string title: ""
    property string description: ""

    signal settingChanged( double value )

    RowLayout {
      anchors.fill: parent
      anchors.leftMargin: InputStyle.panelMargin
      anchors.rightMargin: InputStyle.panelMargin
      anchors.topMargin: InputStyle.innerFieldMargin
      anchors.bottomMargin: InputStyle.innerFieldMargin

      ColumnLayout {
         Layout.preferredWidth: parent.width * 0.5
         Layout.maximumWidth: parent.width * 0.6
         Layout.fillHeight: true

         spacing: InputStyle.panelSpacing

         Text {
             Layout.fillWidth: true
             Layout.preferredHeight: parent.height / 3

             text: root.title

             horizontalAlignment: Text.AlignLeft
             verticalAlignment: Text.AlignVCenter

             elide: Text.ElideRight

             color: InputStyle.fontColor
             font.pixelSize: InputStyle.fontPixelSizeNormal
         }

         Text {
             Layout.fillWidth: true
             Layout.fillHeight: true

             text: root.description

             verticalAlignment: Text.AlignVCenter
             horizontalAlignment: Text.AlignLeft

             color: InputStyle.secondaryFontColor
             font.pixelSize: InputStyle.fontPixelSizeSmall

             wrapMode: Text.Wrap
             maximumLineCount: 2
             elide: Text.ElideRight
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
