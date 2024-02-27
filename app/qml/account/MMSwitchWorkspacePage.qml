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

import "../inputs"
import "../components"

Page {
  id: root

  signal back
  signal createWorkspaceRequested()

  header: MMHeader {
    width: root.width
    title: qsTr( "Select Workspace" )
    onBackClicked: root.back()

    rightMarginShift: addButton.width
    backVisible: true
    color: __style.lightGreenColor

    MMRoundButton {
      id: addButton

      anchors.right: parent.right
      anchors.rightMargin: __style.pageMargins
      anchors.verticalCenter: parent.verticalCenter

      iconSource: __style.addIcon
      iconColor: __style.forestColor

      bgndColor: __style.grassColor
      bgndHoverColor: __style.mediumGreenColor

      onClicked: root.createWorkspaceRequested()
    }
  }

  background: Rectangle { color: __style.lightGreenColor }

  MMSearchInput {
    id: searchBar

    anchors {
      top: parent.top
      topMargin: __style.margin20
      left: parent.left
      leftMargin: __style.pageMargins
      right: parent.right
      rightMargin: __style.pageMargins
    }

    onSearchTextChanged: function( searchText ) {
      wsProxyModel.searchExpression = searchText
    }
  }

  ScrollView {

    anchors {
      top: searchBar.bottom
      topMargin: __style.margin40
      left: parent.left
      leftMargin: __style.pageMargins
      right: parent.right
      rightMargin: __style.pageMargins
    }

    contentWidth: availableWidth // only scroll vertically

    ColumnLayout {
      anchors {
        fill: parent
      }

      // invitations
      Label {
        Layout.fillWidth: true
        Layout.preferredHeight: __style.row24

        text: qsTr("You have a pending invitation")

        font: __style.p6
        color: __style.nightColor

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
          Layout.preferredHeight: __style.row80

          border.color: __style.whiteColor
          radius: __style.inputRadius

          ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Label {
              text: model.display

              Layout.fillWidth: true
              Layout.preferredHeight: parent.height / 2

              wrapMode: Text.Wrap
              color: __style.nightColor
              font: __style.t3

              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignVCenter
            }

            RowLayout {

              Layout.fillWidth: true
              Layout.preferredHeight: __style.row40
              spacing: __style.margin12

              MMRoundButton {
                // Reject

                Layout.preferredWidth: invDelegate.width / 2
                Layout.preferredHeight: __style.row40

                iconSource: __style.closeIcon
                iconColor: __style.grapeColor

                bgndColor: __style.negativeColor

                onClicked: {
                  __merginApi.processInvitation( model.whatsThis, false )
                }
              }

              MMRoundButton {
                // Accept

                Layout.preferredWidth: invDelegate.width / 2
                Layout.preferredHeight: __style.row40

                iconSource: __style.checkmarkIcon
                iconColor: __style.forestColor

                bgndColor: __style.positiveColor

                onClicked: {
                  __merginApi.processInvitation( model.whatsThis, true )
                }
              }
            }
          }
        }
      }

      // workspaces
      Label {
        Layout.fillWidth: true
        Layout.topMargin: __style.margin40
        Layout.preferredHeight: __style.row24

        text: qsTr("Workspaces")

        font: __style.p6
        color: __style.nightColor

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
          Layout.preferredHeight: __style.row40

          radius: __style.inputRadius
          color: isActive ? __style.forestColor : __style.whiteColor

          ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Label {
              text: model.display

              Layout.fillWidth: true
              Layout.leftMargin: __style.margin12
              Layout.fillHeight: true

              wrapMode: Text.Wrap
              color: isActive ? __style.whiteColor : __style.nightColor
              font: __style.t3

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
    }
  }
}
