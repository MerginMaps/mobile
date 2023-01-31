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
import QtQuick.Layouts
import lc 1.0

import "."
import "./components"
import "./misc"

Page {
  id: root

  signal back
  signal createWorkspaceRequested

  property bool haveBack: false
  property bool showCreate: true

  header: PanelHeaderV2 {
    width: root.width
    headerTitle: qsTr("Join a workspace")
    onBackClicked: root.back()
    haveBackButton: root.haveBack
  }

  ColumnLayout {
    anchors {
      fill: parent
      leftMargin: InputStyle.outerFieldMargin
      rightMargin: InputStyle.outerFieldMargin
    }

    Image {
      Layout.alignment: Qt.AlignHCenter

      width: parent.width / 2
      source: InputStyle.mmLogoHorizontal
      sourceSize.width: width
    }

    Rectangle {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.xSmallGap
      color: "transparent"
    }

    Label {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.rowHeightSmall

      text: qsTr("Hello") + " " + __merginApi.userAuth.username + "!"

      font.pixelSize: InputStyle.fontPixelSizeNormal
      font.bold: true
      color: InputStyle.fontColor

      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
    }

    Label {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.rowHeightSmall

      text: qsTr("You have been invited to the following workspaces:")

      font.pixelSize: InputStyle.fontPixelSizeNormal
      font.bold: true
      color: InputStyle.fontColor

      wrapMode: Text.WordWrap
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.xSmallGap
      color: "transparent"
    }

    ScrollView {
      Layout.fillWidth: true
      Layout.fillHeight: true

      contentWidth: availableWidth // only scroll vertically

      ColumnLayout {
        anchors {
          fill: parent
          leftMargin: InputStyle.outerFieldMargin
          rightMargin: InputStyle.outerFieldMargin
        }

        Repeater {
          id: invRepeater

          // TODO: change model to userInfo -> invitations
          model: InvitationsProxyModel {
            invitationsSourceModel: InvitationsModel {
              merginApi: __merginApi
            }
          }

          delegate: Rectangle {
            id: invDelegate

            Layout.fillWidth: true
            Layout.preferredHeight: InputStyle.rowHeightListClickable

            border.color: InputStyle.panelBackgroundLight
            border.width: InputStyle.borderSize
            radius: InputStyle.cornerRadius

            RowLayout {
              anchors.fill: parent
              anchors.leftMargin: InputStyle.panelMargin
              anchors.rightMargin: InputStyle.panelMargin

              spacing: 0

              Label {
                text: qsTr("Workspace") + ": " + model.display

                Layout.fillWidth: true
                Layout.fillHeight: true

                wrapMode: Text.Wrap
                color: InputStyle.fontColor
                font.pixelSize: InputStyle.fontPixelSizeBig
                font.bold: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
              }

              DelegateButton {
                Layout.preferredWidth: invDelegate.width / 3
                Layout.preferredHeight: InputStyle.mediumBtnHeight

                btnWidth: width
                btnHeight: InputStyle.mediumBtnHeight

                bgColor: InputStyle.clrPanelBackground

                fontPixelSize: InputStyle.fontPixelSizeSmall

                text: qsTr("Accept")

                onClicked: {
                  __merginApi.processInvitation( model.whatsThis, true )
                }
              }
            }
          }
        }

        Rectangle {
          Layout.fillWidth: true
          Layout.preferredHeight: InputStyle.xSmallGap
          color: "transparent"
        }

        Text {
          Layout.fillWidth: true
          Layout.preferredHeight: InputStyle.rowHeightHeader

          color: InputStyle.fontColor
          linkColor: InputStyle.highlightColor

          wrapMode: Text.WordWrap
          textFormat: Text.StyledText

          font.bold: true
          font.pixelSize: InputStyle.fontPixelSizeNormal

          verticalAlignment: Qt.AlignVCenter
          horizontalAlignment: Qt.AlignHCenter

          visible: root.showCreate && !__purchasing.transactionPending

          text: ("%1" + qsTr("Do you want to create a new workspace instead?") + "\n" + qsTr("Click here!") + "%2")
            .arg("<a href='http://click-me'>")
            .arg("</a>")

          onLinkActivated: root.createWorkspaceRequested()
        }

        Rectangle {
          Layout.fillWidth: true
          Layout.preferredHeight: InputStyle.xSmallGap
          color: "transparent"
        }
      }
    }
  }
}
