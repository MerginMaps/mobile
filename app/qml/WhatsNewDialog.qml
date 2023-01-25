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
import QtQuick.Dialogs
import QtQuick.Layouts

import "."
import "./components"

Dialog {
  property string informativeText
  property string infoUrl

  id: root
  visible: false
  modal: true
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
  anchors.centerIn: parent
  margins: InputStyle.outerFieldMargin

  title: qsTr("What's new")

  background: Rectangle {
    anchors.fill: parent
    color: "white"
  }

  contentItem: ColumnLayout {
    anchors.margins: InputStyle.outerFieldMargin
    anchors.fill: parent
    spacing: 0

    Text {
      id: infoText

      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.rowHeight * 2

      text: root.informativeText
      wrapMode: Text.WordWrap
      color: InputStyle.fontColor
      font.pixelSize: InputStyle.fontPixelSizeNormal
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignJustify
    }

    RowLayout {
      Layout.fillWidth: true

      spacing: InputStyle.formSpacing

      DelegateButton {
        Layout.preferredWidth: root.width / 2
        Layout.preferredHeight: InputStyle.mediumBtnHeight

        text: qsTr("Ignore")

        onClicked: {
          __appSettings.ignoreWhatsNew = true
          root.close()
        }
      }
 
      DelegateButton {
        Layout.preferredWidth: root.width / 2
        Layout.preferredHeight: InputStyle.mediumBtnHeight

        text: qsTr("More info")

        onClicked: {
          Qt.openUrlExternally(root.infoUrl)
          root.close()
        }
      }
    }
  }
}
