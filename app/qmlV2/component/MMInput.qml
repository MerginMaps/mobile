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

Item {
  id: control

  enum Type { Normal, Password, Search, Calendar, Scan, CopyButton }

  property int type: MMInput.Type.Normal
  property alias text: textField.text
  property alias placeholderText: textField.placeholderText
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

        source: visible ? StyleV2.errorIcon : ""
        color: errorMsg.length > 0 ? StyleV2.negativeColor : StyleV2.warningColor
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
      Text {
        text: errorMsg.length > 0 ? errorMsg : warningMsg
        font: StyleV2.t4
        wrapMode: Text.WordWrap
        width: messageItem.width - msgRow.spacing - msgIcon.width
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
    }
  }

  Rectangle {
    id: rect

    height: 40 * __dp
    width: parent.width
    color: (errorMsg.length > 0 || warningMsg.length > 0) ? StyleV2.errorBgInputColor : StyleV2.whiteColor
    border.color: (textField.activeFocus || textField.hovered) ? (errorMsg.length > 0 ? StyleV2.negativeColor :
                                                                                        warningMsg.length > 0 ? StyleV2.warningColor :
                                                                                                                StyleV2.forestColor) : StyleV2.transparentColor
    border.width: enabled ? (textField.activeFocus ? 2*__dp : 1*__dp) : 0
    radius: parent.height

    Row {
      id: row

      anchors.verticalCenter: parent.verticalCenter
      leftPadding: 10 * __dp

      MMIcon {
        id: leftIcon

        source: control.type === MMInput.Type.Search ? StyleV2.searchIcon :
                                                       control.type === MMInput.Type.Calendar ? StyleV2.calendarIcon : ""
        color: errorMsg.length > 0 ? StyleV2.negativeColor :
                                     warningMsg.length > 0 ? StyleV2.warningColor :
                                                             control.enabled ? StyleV2.forestColor : StyleV2.mediumGreenColor
        width: height
        height: rect.height
        visible: control.type === MMInput.Type.Search || control.type === MMInput.Type.Calendar
      }

      TextField {
        id: textField

        y: 2 * __dp
        width: control.width - 2 * row.leftPadding
               - (leftIcon.visible ? leftIcon.width : 0)
               - (rightIcon.visible ? rightIcon.width : 0)
               - (button.visible ? button.width : 0)
        height: rect.height - 4 * __dp
        color: control.enabled ? StyleV2.nightColor : StyleV2.mediumGreenColor
        placeholderTextColor: StyleV2.nightAlphaColor
        font: StyleV2.p5
        hoverEnabled: true
        anchors.verticalCenter: parent.verticalCenter
        echoMode: (control.type === MMInput.Type.Password && !rightIcon.pressed) ? TextInput.Password : TextInput.Normal
        background: Rectangle {
          color: StyleV2.transparentColor
        }
      }

      MMIcon {
        id: rightIcon

        property bool pressed: false
        source: control.type === MMInput.Type.Password ? (pressed ? StyleV2.hideIcon : StyleV2.showIcon) :
                                                         control.type === MMInput.Type.Scan ? StyleV2.qrCodeIcon :
                                                                                              (textField.activeFocus && textField.text.length>0) ? StyleV2.xMarkIcon : ""
        color: control.enabled ? StyleV2.forestColor : StyleV2.mediumGreenColor
        width: visible ? height : 0
        height: rect.height
        visible: control.type === MMInput.Type.Password ||
                 control.type === MMInput.Type.Scan ||
                 ((control.type !== MMInput.Type.CopyButton) && textField.activeFocus && textField.text.length>0)

        MouseArea {
          anchors.fill: parent
          onClicked: click()

          function click() {
            if(control.type === MMInput.Type.Password) {
              rightIcon.pressed = !rightIcon.pressed
            }
            else if(textField.activeFocus && textField.text.length>0) {
              textField.text = ""
            }
          }
        }
      }

      Button {
        id: button

        visible: control.type === MMInput.Type.CopyButton
        anchors.verticalCenter: parent.verticalCenter

        contentItem: Text {
          anchors.centerIn: button
          font: StyleV2.t5
          text: qsTr("Copy")
          leftPadding: 2 * __dp
          rightPadding: 2 * __dp
          topPadding: 2 * __dp
          bottomPadding: 2 * __dp
          color: StyleV2.deepOceanColor
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideRight
        }

        background: Rectangle {
          color: button.enabled ? StyleV2.grassColor : StyleV2.mediumGreenColor
          radius: height / 2
        }

        onClicked: {
          textField.selectAll()
          textField.copy()
          textField.deselect()
        }
      }

    }
  }
}
