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

Column {
  id: control

  signal textEdited

  enum MsgShowBehaviour {
      Never,
      OnNotMatchingRegex,
      Always
  }

  property alias title: titleItem.text
  property alias text: textField.text
  property alias placeholderText: textField.placeholderText
  property string regexp: '.*'
  property int msgShowBehaviour: MMPasswordInput.OnNotMatchingRegex
  property url iconSource: ""
  property string warningMsg
  property string errorMsg

  spacing: 6 * __dp
  width: 280 * __dp

  Text {
    id: titleItem

    width: parent.width
    font: __style.p6
    wrapMode: Text.WordWrap
    visible: text.length > 0
  }

  Rectangle {
    id: rect

    height: 40 * __dp
    width: parent.width
    color: (errorMsg.length > 0 || warningMsg.length > 0) ? __style.errorBgInputColor : __style.whiteColor
    border.color: isPasswordCorrect(textField.text) ? __style.forestColor : errorMsg.length > 0 ? __style.negativeColor : warningMsg.length > 0 ? __style.warningColor : __style.forestColor
    border.width: enabled ? (textField.activeFocus ? 2*__dp : textField.hovered ? 1*__dp : 0) : 0
    radius: parent.height

    Row {
      id: row

      anchors.verticalCenter: parent.verticalCenter
      leftPadding: 10 * __dp

      MMIcon {
        id: leftIcon

        source: control.iconSource
        color: errorMsg.length > 0 ? __style.negativeColor :
                                     warningMsg.length > 0 ? __style.warningColor :
                                                             control.enabled ? __style.forestColor : __style.mediumGreenColor
        height: rect.height
      }

      TextField {
        id: textField

        y: 2 * __dp
        width: control.width - 2 * row.leftPadding
               - (leftIcon.visible ? leftIcon.width : 0)
               - (eyeButton.visible ? eyeButton.width : 0)
        height: rect.height - 4 * __dp
        color: control.enabled ? __style.nightColor : __style.mediumGreenColor
        placeholderTextColor: __style.nightAlphaColor
        font: __style.p5
        hoverEnabled: true
        anchors.verticalCenter: parent.verticalCenter
        echoMode: eyeButton.pressed ? TextInput.Normal : TextInput.Password
        background: Rectangle {
          color: __style.transparentColor
        }

        onTextEdited: {
            control.textEdited()
        }
      }

      MMIcon {
        id: eyeButton

        property bool pressed: false
        source: pressed ? __style.hideIcon : __style.showIcon
        color: control.enabled ? __style.forestColor : __style.mediumGreenColor
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

        source: visible ? __style.errorIcon : ""
        color: errorMsg.length > 0 ? __style.negativeColor : __style.warningColor
        visible: msg.visible
      }
      Text {
        id: msg

        text: errorMsg.length > 0 ? errorMsg : warningMsg
        font: __style.t4
        wrapMode: Text.WordWrap
        width: messageItem.width - msgRow.spacing - msgIcon.width
        visible: (errorMsg.length > 0 || warningMsg.length > 0) && !isPasswordCorrect(textField.text)
      }
    }
  }

  function isPasswordCorrect(pwd) {
    if (msgShowBehaviour === MMPasswordInput.Never)
        return true
    else if (msgShowBehaviour === MMPasswordInput.Always)
        return false

    let pwdRegexp = new RegExp(control.regexp)
    return pwdRegexp.test(pwd)
  }
}
