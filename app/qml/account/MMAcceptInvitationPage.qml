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

MMPage {
  id: root

  required property var invitation /* MM.MerginInvitation */

  property bool haveBack: false
  property bool showCreate: true

  signal joinWorkspaceClicked(string workspaceUuid)
  signal createWorkspaceClicked

  header: null

  pageBottomMarginPolicy: MMPage.BottomMarginPolicy.PaintBehindSystemBar

  pageContent: MMScrollView {
    id: scrollContent

    width: parent.width
    height: parent.height

    Column {
      width: parent.width

      MMListSpacer {
        height: Math.max( ( scrollContent.height - contentMessage.height - footerSpacer.height - buttonsGroup.height ) / 2, __style.margin20 )
      }

      Item {
        id: contentMessage

        height: childrenRect.height
        width: parent.width

        Column {
          id: messageColumn
          width: parent.width

          spacing: __style.margin36

          MMMessage {
            width: parent.width

            image: __style.positiveMMSymbolImage
            title: qsTr("You have been invited to a workspace")
            description: qsTr("It is better to work together, join the workspace and explore Mergin Maps!")
          }

          MMText {
            width: parent.width
            text: root.invitation.workspace
            font: __style.t1
            color: __style.nightColor
            wrapMode: Text.Wrap
            elide: Text.ElideRight
            maximumLineCount: 2
            horizontalAlignment: Text.AlignHCenter
          }
        }
      }

      MMListSpacer {
        height: Math.max( ( scrollContent.height - contentMessage.height - footerSpacer.height - buttonsGroup.height ) / 2, __style.margin20 )
      }

      Item {
        id: buttonsGroup

        width: parent.width
        height: childrenRect.height

        Column {

          width: parent.width
          spacing: __style.spacing20

          MMButton {
            width: parent.width
            text: qsTr("Join workspace")

            onClicked: root.joinWorkspaceClicked( root.invitation.uuid )
          }

          MMHlineText {
            width: parent.width
            title: qsTr( "or" )
            visible: root.showCreate
          }

          Text {
            width: parent.width

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

      MMListFooterSpacer { id: footerSpacer }
    }
  }
}
