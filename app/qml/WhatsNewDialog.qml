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

import "."
import "./components"

Dialog {
  id: root

  property string infoUrl
  property string informativeText

  modal: true
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

  title: qsTr("What's new")

  background: Rectangle {
    radius: InputStyle.cornerRadius
    color: "white"
  }

  contentItem: ColumnLayout {

    Text {
      id: infoText

      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.minimumHeight: InputStyle.rowHeightHeader

      text: root.informativeText
      wrapMode: Text.WordWrap
      color: InputStyle.fontColor
      font.pixelSize: InputStyle.fontPixelSizeNormal
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignJustify
    }


    RowLayout {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.rowHeightHeader

      DelegateButton {
        Layout.fillWidth: true
        Layout.preferredHeight: InputStyle.mediumBtnHeight

        text: qsTr("Ignore")

        onClicked: {
          __appSettings.ignoreWhatsNew = true
          root.close()
        }
      }
 
      DelegateButton {
        Layout.fillWidth: true
        Layout.preferredHeight: InputStyle.mediumBtnHeight

        text: qsTr("Read more")

        onClicked: {
          Qt.openUrlExternally(root.infoUrl)
          root.close()
        }
      }
    }
  }
}
