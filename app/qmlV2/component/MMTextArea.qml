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
    placeholderTextColor: __style.nightAlphaColor
    color: control.enabled ? __style.nightColor : __style.mediumGreenColor
    font: __style.p5

    background: Rectangle {
      color: (errorMsg.length > 0 || warningMsg.length > 0) ? __style.errorBgInputColor : __style.whiteColor
      border.color: (textArea.activeFocus || textArea.hovered) ? (errorMsg.length > 0 ? __style.negativeColor :
                                                                                        warningMsg.length > 0 ? __style.warningColor :
                                                                                                                __style.forestColor) : __style.transparentColor
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

        source: visible ? __style.errorIcon : ""
        color: errorMsg.length > 0 ? __style.negativeColor : __style.warningColor
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
      Text {
        text: errorMsg.length > 0 ? errorMsg : warningMsg
        font: __style.t4
        wrapMode: Text.WordWrap
        width: messageItem.width - msgRow.spacing - msgIcon.width
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
    }
  }
}
