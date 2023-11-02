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

Item {
  id: control

  property alias text: textField.text
  property alias placeholderText: textField.placeholderText
  required property string regexp
  property url iconSource: ""
  property string warningMsg
  property string errorMsg

  width: 280 * __dp
  height: rect.height + messageItem.height

  Item {
    id: messageItem

    width: parent.width
    anchors.left: parent.left
    anchors.top: rect.bottom
    anchors.topMargin: 6 * __dp
    height: msgRow.height

    Row {
      id: msgRow

      spacing: 4 * __dp

      MMIcon {
        id: msgIcon

        source: visible ? Style.errorIcon : ""
        color: errorMsg.length > 0 ? Style.negative : Style.warning
        visible: msg.visible
      }
      Text {
        id: msg

        text: errorMsg.length > 0 ? errorMsg : warningMsg
        font: Qt.font(Style.t4)
        wrapMode: Text.WordWrap
        width: messageItem.width - msgRow.spacing - msgIcon.width
        visible: (errorMsg.length > 0 || warningMsg.length > 0) && !isPasswordCorrect(textField.text)
      }
    }
  }

  Rectangle {
    id: rect

    height: 40 * __dp
    width: parent.width
    color: (errorMsg.length > 0 || warningMsg.length > 0) ? Style.errorBgInputColor : Style.white
    border.color: isPasswordCorrect(textField.text) ? Style.forest : errorMsg.length > 0 ? Style.negative : warningMsg.length > 0 ? Style.warning : Style.forest
    border.width: enabled ? (textField.activeFocus ? 2*__dp : textField.hovered ? 1*__dp : 0) : 0
    radius: parent.height

    Row {
      id: row

      anchors.verticalCenter: parent.verticalCenter
      leftPadding: 10 * __dp

      MMIcon {
        id: leftIcon

        source: control.iconSource
        color: errorMsg.length > 0 ? Style.negative :
                                     warningMsg.length > 0 ? Style.warning :
                                                             control.enabled ? Style.forest : Style.mediumGreen
        height: rect.height
      }

      TextField {
        id: textField

        y: 2 * __dp
        width: control.width - 2 * row.leftPadding
               - (leftIcon.visible ? leftIcon.width : 0)
               - (eyeButton.visible ? eyeButton.width : 0)
        height: rect.height - 4 * __dp
        color: control.enabled ? Style.night : Style.mediumGreen
        placeholderTextColor: Style.night_6
        font: Qt.font(Style.p5)
        hoverEnabled: true
        anchors.verticalCenter: parent.verticalCenter
        echoMode: eyeButton.pressed ? TextInput.Normal : TextInput.Password
        background: Rectangle {
          color: Style.transparent
        }
      }

      MMIcon {
        id: eyeButton

        property bool pressed: false
        source: pressed ? Style.hideIcon : Style.showIcon
        color: control.enabled ? Style.forest : Style.mediumGreen
        width: visible ? height : 0
        height: rect.height

        MouseArea {
          anchors.fill: parent
          onClicked: eyeButton.pressed = !eyeButton.pressed
        }
      }
    }
  }

  function isPasswordCorrect(pwd) {
    let pwdRegexp = new RegExp(control.regexp)
    return pwdRegexp.test(pwd)
  }
}
