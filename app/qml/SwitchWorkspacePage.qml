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
  signal createWorkspaceRequested()

  header: PanelHeaderV2 {
    width: root.width
    headerTitle: qsTr( "Select a workspace" )
    onBackClicked: root.back()
  }

  SearchBoxV2 {
    id: searchBar

    anchors {
      top: parent.top
      topMargin: InputStyle.panelMarginV2
      left: parent.left
      leftMargin: InputStyle.outerFieldMargin
      right: parent.right
      rightMargin: InputStyle.outerFieldMargin
    }

    onSearchTextChanged: function( searchText ) {
      wsProxyModel.searchExpression = searchText
    }
  }

  ScrollView {

    anchors {
      top: searchBar.bottom
      left: parent.left
      right: parent.right
      bottom: parent.bottom
    }

    contentWidth: availableWidth // only scroll vertically

    ColumnLayout {
      anchors {
        fill: parent
        leftMargin: InputStyle.outerFieldMargin
        rightMargin: InputStyle.outerFieldMargin
      }

      Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: InputStyle.xSmallGap
        color: "transparent"
      }

      // invitations
      Label {
        Layout.fillWidth: true
        Layout.preferredHeight: InputStyle.rowHeightSmall

        text: qsTr("You have a pending invitation")

        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor

        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter

        visible: invRepeater.count > 0
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

      // workspaces

      Label {
        Layout.fillWidth: true
        Layout.topMargin: InputStyle.xSmallGap
        Layout.preferredHeight: InputStyle.rowHeightSmall

        text: qsTr("Workspaces")

        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor

        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter

        visible: invRepeater.count > 0
      }

      Repeater {
        model: WorkspacesProxyModel {
          id: wsProxyModel

          workspacesSourceModel: WorkspacesModel {
            merginApi: __merginApi
          }
        }

        delegate: Rectangle {

          property bool isActive: model.whatsThis === __merginApi.userInfo.activeWorkspaceId

          Layout.fillWidth: true
          Layout.preferredHeight: InputStyle.rowHeightListEntry

          radius: InputStyle.cornerRadius
          color: isActive ? InputStyle.fontColorBright : InputStyle.clrPanelMain

          ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Label {
              text: model.display

              Layout.fillWidth: true
              Layout.leftMargin: InputStyle.panelSpacing
              Layout.fillHeight: true

              wrapMode: Text.Wrap
              color: isActive ? InputStyle.clrPanelMain : InputStyle.fontColor
              font.pixelSize: InputStyle.fontPixelSizeBig
              font.bold: true

              horizontalAlignment: Text.AlignLeft
              verticalAlignment: Text.AlignVCenter
            }
          }

          MouseArea {
            anchors.fill: parent
            onClicked: function( mouse ) {
              mouse.accepted = true
              __merginApi.userInfo.setActiveWorkspace( model.whatsThis )
              root.back()
            }
          }
        }
      }

      DelegateButton {
        id: createWorkspaceButton

        Layout.fillWidth: true
        Layout.preferredHeight: InputStyle.fieldHeight
        Layout.bottomMargin: InputStyle.panelMarginV2

        btnWidth: width / 2

        text: qsTr("Create new workspace")
        onClicked: {
          root.createWorkspaceRequested()
        }
      }
    }
  }
}
