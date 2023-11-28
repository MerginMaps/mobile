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
import ".."

Column {
  id: control

  property alias title: titleItem.text
  property alias text: textField.text
  property alias placeholderText: textField.placeholderText
  required property string regexp
  property url iconSource: ""
  property string warningMsg
  property string errorMsg

  spacing: 6 * __dp
  width: 280 * __dp

  Text {
    id: titleItem

    width: parent.width
    font: StyleV2.p6
    wrapMode: Text.WordWrap
    visible: text.length > 0
  }

  Rectangle {
    id: rect

    height: 40 * __dp
    width: parent.width
    color: (errorMsg.length > 0 || warningMsg.length > 0) ? StyleV2.errorBgInputColor : StyleV2.whiteColor
    border.color: isPasswordCorrect(textField.text) ? StyleV2.forestColor : errorMsg.length > 0 ? StyleV2.negativeColor : warningMsg.length > 0 ? StyleV2.warningColor : StyleV2.forestColor
    border.width: enabled ? (textField.activeFocus ? 2*__dp : textField.hovered ? 1*__dp : 0) : 0
    radius: parent.height

    Row {
      id: row

      anchors.verticalCenter: parent.verticalCenter
      leftPadding: 10 * __dp

      MMIcon {
        id: leftIcon

        source: control.iconSource
        color: errorMsg.length > 0 ? StyleV2.negativeColor :
                                     warningMsg.length > 0 ? StyleV2.warningColor :
                                                             control.enabled ? StyleV2.forestColor : StyleV2.mediumGreenColor
        height: rect.height
      }

      TextField {
        id: textField

        y: 2 * __dp
        width: control.width - 2 * row.leftPadding
               - (leftIcon.visible ? leftIcon.width : 0)
               - (eyeButton.visible ? eyeButton.width : 0)
        height: rect.height - 4 * __dp
        color: control.enabled ? StyleV2.nightColor : StyleV2.mediumGreenColor
        placeholderTextColor: StyleV2.nightAlphaColor
        font: StyleV2.p5
        hoverEnabled: true
        anchors.verticalCenter: parent.verticalCenter
        echoMode: eyeButton.pressed ? TextInput.Normal : TextInput.Password
        background: Rectangle {
          color: StyleV2.transparentColor
        }
      }

      MMIcon {
        id: eyeButton

        property bool pressed: false
        source: pressed ? StyleV2.hideIcon : StyleV2.showIcon
        color: control.enabled ? StyleV2.forestColor : StyleV2.mediumGreenColor
        width: visible ? height : 0
        height: rect.height

        MouseArea {
          anchors.fill: parent
          onClicked: eyeButton.pressed = !eyeButton.pressed
        }
      }
    }
  }

  Item {
    id: messageItem

    width: parent.width
    height: msgRow.height

    Row {
      id: msgRow

      spacing: 4 * __dp

      MMIcon {
        id: msgIcon

        source: visible ? StyleV2.errorIcon : ""
        color: errorMsg.length > 0 ? StyleV2.negativeColor : StyleV2.warningColor
        visible: msg.visible
      }
      Text {
        id: msg

        text: errorMsg.length > 0 ? errorMsg : warningMsg
        font: StyleV2.t4
        wrapMode: Text.WordWrap
        width: messageItem.width - msgRow.spacing - msgIcon.width
        visible: (errorMsg.length > 0 || warningMsg.length > 0) && !isPasswordCorrect(textField.text)
      }
    }
  }

  function isPasswordCorrect(pwd) {
    let pwdRegexp = new RegExp(control.regexp)
    return pwdRegexp.test(pwd)
  }
}
