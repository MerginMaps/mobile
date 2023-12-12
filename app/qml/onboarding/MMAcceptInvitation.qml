/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../components"
import "../inputs"

Page {
  id: root

  required property string user
  required property string workspace

  signal backClicked
  signal continueClicked
  signal createWorkspaceClicked

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  ScrollView {
    width: parent.width
    height: parent.height

    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff

    Column {
      id: mainColumn

      width: root.width
      spacing: 20 * __dp
      leftPadding: 20 * __dp
      rightPadding: 20 * __dp
      topPadding: 100 * __dp
      bottomPadding: 20 * __dp

      Item {
        width: parent.width - mainColumn.leftPadding - mainColumn.rightPadding
        height: bg.height

        Image {
          id: bg

          anchors.horizontalCenter: parent.horizontalCenter
          source: __style.acceptInvitationImage

          Image {
            id: fg
            x: ( bg.width - fg.width ) / 2 + 7
            y: ( bg.height - fg.height ) / 2 + 5
            source: __style.acceptInvitationLogoImage
          }
        }
      }

      Text {
        width: parent.width - mainColumn.leftPadding - mainColumn.rightPadding
        text: qsTr("You have been invited to workspace")
        font: __style.h3
        color: __style.forestColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.2
      }

      Text {
        width: parent.width - mainColumn.leftPadding - mainColumn.rightPadding
        text: qsTr("User %1 has invited you to join his workspace").arg(root.user)
        font: __style.p5
        color: __style.nightColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.5
      }

      Item { width: 1; height: 1 }

      Text {
        width: parent.width - mainColumn.leftPadding - mainColumn.rightPadding
        text: root.workspace
        font: __style.t1
        color: __style.nightColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.5
      }

      Item { width: 1; height: 50 }

      MMButton {
        width: parent.width - mainColumn.leftPadding - mainColumn.rightPadding
        text: qsTr("Join workspace")

        onClicked: root.continueClicked()
      }

      MMHlineText {
        width: parent.width - mainColumn.leftPadding - mainColumn.rightPadding
        title: qsTr("or")
      }

      MMLinkButton {
        width: parent.width - mainColumn.leftPadding - mainColumn.rightPadding
        text: qsTr("Create new workspace")

        onClicked: root.createWorkspaceClicked()
      }
    }
  }
}
