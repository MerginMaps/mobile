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

import "../components"
import "../inputs"

Drawer {
  id: root

  required property string username

  signal closeAccountClicked()

  width: window.width // TODO maximum size
  height: mainColumn.height
  edge: Qt.BottomEdge
  dim: true
  interactive: false
  dragMargin: 0
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

  Rectangle {
    color: roundedRect.color
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: 2 * radius
    anchors.topMargin: -radius
    radius: 20 * __dp
  }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: __style.whiteColor

    Column {
      id: mainColumn

      width: parent.width - 2* __style.pageMargins
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: 20 * __dp
      leftPadding: 20 * __dp
      rightPadding: 20 * __dp
      bottomPadding: 20 * __dp

      MMPageHeader {
        width: parent.width
        backVisible: false

        MMRoundButton {
          id: closeButton

          anchors.right: parent.right
          anchors.rightMargin: __style.pageMargins
          bgndColor: __style.lightGreenColor
          iconSource: __style.closeIcon
          onClicked: root.close()
        }
      }

      MMMessage {
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        image: __style.closeAccountImage
        title: qsTr("Do you really wish to close your account?")
        description: qsTr("This action will delete your Mergin Maps account. If you are a workspace owner, you need to transfer the ownership to somebody else or close the workspace.")
      }

      Text {
        id: usernameText

        text: root.username
        anchors.horizontalCenter: parent.horizontalCenter

        font: __style.t1
        color: __style.nightColor
      }

      MMTextInput {
        id: usernameInput

        width: parent.width
        bgColor: __style.lightGreenColor
        anchors.horizontalCenter: parent.horizontalCenter
        title: qsTr("Username")
        placeholderText: qsTr("Enter your username")
      }

      Item { width: 1; height: 1 }

      MMTextBubble {
        id: noteBubble

        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter

        title: qsTr("Please note")
        description: qsTr("Type in your username and click ‘Yes’ to delete your account.")

        bgColor: __style.lightGreenColor
        image: __style.attentionImage
      }

      Row {
        width: parent.width
        spacing: __style.pageMargins
        anchors.horizontalCenter: parent.horizontalCenter

        MMLinkButton {
          id: noButton

          width: parent.width / 2 - __style.pageMargins
          text: qsTr("No")

          onClicked: {
            close()
          }
        }

        MMButton {
          id: yesButton

          width: noButton.width
          text: qsTr("Yes")

          colorPrimary: __style.grapeColor
          colorSecondary: __style.negativeColor

          onClicked: {
            if (usernameInput.text === root.username)
              root.closeAccountClicked()
            else
              usernameInput.errorMsg = qsTr("Usernames do not match.")
          }
        }
      }
    }
  }
}
