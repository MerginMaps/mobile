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
import QtQuick.Controls.Basic
import "../Style.js" as Style
import ".."

Item {
  id: control

  property int areaHeight
  property alias text: textArea.text
  property alias placeholderText: textArea.placeholderText
  property string warningMsg
  property string errorMsg

  // auto resize
  property int minHeight: 50
  property int maxHeight: 150
  property bool autoHeight: false

  width: 280 * __dp
  height: textArea.height + messageItem.height

  TextArea {
    id: textArea

    property string warningMsg
    property string errorMsg

    height: control.autoHeight ? ( contentHeight+10 > control.minHeight ? contentHeight+10 > control.maxHeight ? control.maxHeight : contentHeight+10 : control.minHeight ) : control.areaHeight
    width: parent.width
    hoverEnabled: true
    placeholderTextColor: StyleV2.nightAlphaColor
    color: control.enabled ? Style.night : Style.mediumGreen
    font: Qt.font(Style.p5)

    background: Rectangle {
      color: (errorMsg.length > 0 || warningMsg.length > 0) ? Style.errorBgInputColor : Style.white
      border.color: (textArea.activeFocus || textArea.hovered) ? (errorMsg.length > 0 ? Style.negative :
                                                                                      warningMsg.length > 0 ? Style.warning :
                                                                                                              Style.forest) : "transparent"
      border.width: enabled ? (textArea.activeFocus ? 2*__dp : 1*__dp) : 0
      radius: 10 * __dp
    }
  }

  Item {
    id: messageItem

    width: parent.width
    anchors.left: parent.left
    anchors.top: textArea.bottom
    anchors.topMargin: 6 * __dp
    height: msgRow.height

    Row {
      id: msgRow

      spacing: 4 * __dp

      MMIcon {
        id: msgIcon

        source: visible ? Style.errorIcon : ""
        color: errorMsg.length > 0 ? Style.negative : Style.warning
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
      Text {
        text: errorMsg.length > 0 ? errorMsg : warningMsg
        font: Qt.font(Style.t4)
        wrapMode: Text.WordWrap
        width: messageItem.width - msgRow.spacing - msgIcon.width
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
    }
  }
}
