/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import lc 1.0

import "./components"
import "."  // import InputStyle singleton

Rectangle {
  id: root

  color: InputStyle.clrPanelMain

  property int value
  property string title: ""
  property string description: ""

  signal settingChanged( int value )

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

    ComboBox {
      id: modeComboBox

      Layout.fillWidth: true

      textRole: "text"
      valueRole: "value"

      onActivated: root.settingChanged(currentValue)
      Component.onCompleted: currentIndex = indexOfValue(root.value)

      model: [
          { text: qsTr("Time elapsed"), value: StreamingIntervalType.Time },
          { text: qsTr("Distance traveled"), value: StreamingIntervalType.Distance }
      ]

      contentItem: Text {
        text: modeComboBox.displayText
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
        elide: Text.ElideRight
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
      }

      delegate: ItemDelegate {
        width: modeComboBox.width
        contentItem: Text {
            text: modeComboBox.textRole
                ? (Array.isArray(modeComboBox.model) ? modelData[modeComboBox.textRole] : model[modeComboBox.textRole])
                : modelData
            color: InputStyle.fontColor
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: InputStyle.fontPixelSizeNormal
        }
        highlighted: modeComboBox.highlightedIndex === index
      }

      background: Rectangle {
        color: InputStyle.clrPanelMain
      }

      Binding {
        target: modeComboBox.indicator
        property: "color"
        value: InputStyle.fontColor
      }
    }
  }
}
