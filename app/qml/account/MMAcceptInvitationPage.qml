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

  required property var invitation /* MM.MerginInvitation */

  property bool haveBack: false
  property bool showCreate: true

  signal joinWorkspaceClicked(string workspaceUuid)
  signal createWorkspaceClicked
  signal backClicked

  readonly property real hPadding: width < __style.maxPageWidth
                                   ? 20 * __dp
                                   : (20 + (width - __style.maxPageWidth) / 2) * __dp
  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  MMPageHeader {
    id: header

    backVisible: root.haveBack

    onBackClicked: root.backClicked()
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
      leftPadding: root.hPadding
      rightPadding: root.hPadding
      topPadding: 100 * __dp
      bottomPadding: 20 * __dp

      Item {
        width: parent.width - 2 * root.hPadding
        height: bg.height

        Image {
          id: bg

          anchors.horizontalCenter: parent.horizontalCenter
          source: __style.positiveMMSymbolImage
        }
      }

      Text {
        width: parent.width - 2 * root.hPadding
        text: qsTr("You have been invited to a workspace")
        font: __style.h3
        color: __style.forestColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.2
      }

      Text {
        width: parent.width - 2 * root.hPadding
        text: qsTr("It is better to work together, join the workspace and explore Mergin Maps together!")
        font: __style.p5
        color: __style.nightColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.5
      }

      Item { width: 1; height: 1 }

      Text {
        width: parent.width - 2 * root.hPadding
        text: root.invitation.workspace
        font: __style.t1
        color: __style.nightColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.5
      }

      Item { width: 1; height: 50 }

      MMButton {
        width: parent.width - 2 * root.hPadding
        text: qsTr("Join workspace")

        onClicked: root.joinWorkspaceClicked(root.invitation.uuid)
      }

      MMHlineText {
        width: parent.width - 2 * root.hPadding
        title: qsTr("or")
        visible: root.showCreate
      }

      Text {
        width: parent.width - 2 * root.hPadding
        text: qsTr("Want to create a new workspace instead? %1Click here%2").arg("<a href='internal-signal' style='color:" + __style.forestColor + "'>").arg("</a>")
        visible: root.showCreate
        font: __style.t3
        color: __style.nightColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.2
        textFormat: Text.RichText

        onLinkActivated: function(link) {
          root.createWorkspaceClicked()
        }
      }
    }
  }
}
