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
  property url iconSource: ""
  required property string buttonText
  property string warningMsg
  property string errorMsg

  signal clicked

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

  Rectangle {
    id: rect

    height: 40 * __dp
    width: parent.width
    color: (errorMsg.length > 0 || warningMsg.length > 0) ? Style.errorBgInputColor : Style.white
    border.color: errorMsg.length > 0 ? Style.negative : warningMsg.length > 0 ? Style.warning : Style.forest
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
               - (button.visible ? button.width : 0)
        height: rect.height - 4 * __dp
        color: control.enabled ? Style.night : Style.mediumGreen
        placeholderTextColor: Style.night_6
        font: Qt.font(Style.p5)
        hoverEnabled: true
        anchors.verticalCenter: parent.verticalCenter
        background: Rectangle {
          color: Style.transparent
        }
      }

      Button {
        id: button

        anchors.verticalCenter: parent.verticalCenter

        contentItem: Text {
          anchors.centerIn: button
          font: Qt.font(Style.t5)
          text: control.buttonText
          leftPadding: 2 * __dp
          rightPadding: 2 * __dp
          topPadding: 2 * __dp
          bottomPadding: 2 * __dp
          color: Style.deepOcean
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideRight
        }

        background: Rectangle {
          color: button.enabled ? Style.grass : Style.mediumGreen
          radius: height / 2
        }

        onClicked: control.clicked()
      }
    }
  }

  // add whole text to clipboard
  function textToClipboard() {
    textField.selectAll()
    textField.copy()
    textField.deselect()
  }
}
