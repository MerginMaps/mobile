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

  header: PanelHeaderV2 {
    width: root.width
    headerTitle: qsTr("Join a workspace")
    onBackClicked: root.back()
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

            ColumnLayout {
              anchors.fill: parent
              spacing: 0

              Label {
                text: qsTr("Workspace") + ": " + model.display

                Layout.fillWidth: true
                Layout.preferredHeight: parent.height / 2

                wrapMode: Text.Wrap
                color: InputStyle.fontColor
                font.pixelSize: InputStyle.fontPixelSizeBig
                font.bold: true

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
              }

              RowLayout {

                Layout.fillWidth: true
                Layout.preferredHeight: 50
                Layout.bottomMargin: InputStyle.tinyGap

                spacing: InputStyle.formSpacing

                DelegateButton {
                  Layout.preferredWidth: invDelegate.width / 2
                  Layout.preferredHeight: InputStyle.mediumBtnHeight

                  btnWidth: width - 2 * InputStyle.formSpacing
                  btnHeight: InputStyle.mediumBtnHeight

                  bgColor: InputStyle.clrPanelBackground

                  fontPixelSize: InputStyle.fontPixelSizeSmall

                  text: qsTr("Accept")

                  onClicked: {
                    __merginApi.processInvitation( model.whatsThis, true )
                  }
                }

                DelegateButton {
                  Layout.preferredWidth: invDelegate.width / 2
                  Layout.preferredHeight: InputStyle.mediumBtnHeight

                  btnWidth: width - 2 * InputStyle.formSpacing
                  btnHeight: InputStyle.mediumBtnHeight

                  bgColor: InputStyle.clrPanelMain
                  fontColor: InputStyle.invalidButtonColor

                  fontPixelSize: InputStyle.fontPixelSizeSmall

                  text: qsTr("Reject")

                  onClicked: {
                    __merginApi.processInvitation( model.whatsThis, false )
                  }
                }
              }
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

    Label {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.rowHeightSmall

      text: qsTr("Do you want to instead create a new workspace?")

      font.pixelSize: InputStyle.fontPixelSizeNormal
      font.bold: true
      color: InputStyle.fontColor

      wrapMode: Text.WordWrap
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
    }

    Button {
      id: createWorkspaceButton

      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.rowHeightSmall

      text: qsTr("Click here!")

      font.pixelSize: InputStyle.fontPixelSizeSmall
      font.bold: true

      onClicked: root.createWorkspaceRequested()
      background: Rectangle {
        color: root.bgColor
      }

      contentItem: Text {
        text: createWorkspaceButton.text
        font: createWorkspaceButton.font
        color: InputStyle.highlightColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
      }
    }

    Rectangle {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.xSmallGap
      color: "transparent"
    }
  }
}
