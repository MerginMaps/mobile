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

import mm 1.0 as MM

import "../inputs"
import "../components"
import "./components" as MMAccountComponents

MMPage {
  id: root

  required property var invitationsModel // MM.InvitationsProxyModel
  required property var workspacesModel // MM.WorkspacesProxyModel

  property int activeWorkspaceId

  signal workspaceClicked( int workspaceId )
  signal invitationClicked( string uuid, bool accepted )
  signal searchTextChanged( string text )

  signal createWorkspaceRequested()

  pageHeader.title: qsTr( "Select workspace" )

  pageSpacing: __style.spacing20
  pageBottomMarginPolicy: MMPage.BottomMarginPolicy.PaintBehindSystemBar

  pageHeader.rightItemContent: MMRoundButton {

    anchors.verticalCenter: parent.verticalCenter

    iconSource: __style.addIcon
    iconColor: __style.forestColor

    bgndColor: __style.grassColor
    bgndHoverColor: __style.mediumGreenColor

    onClicked: root.createWorkspaceRequested()
  }

  pageContent: Item {

    width: parent.width
    height: parent.height

    MMSearchInput {
      id: searchBar

      width: parent.width

      placeholderText: qsTr( "Search" ) + "..."

      onSearchTextChanged: function( searchText ) { root.searchTextChanged( searchText ) }
    }

    MMScrollView {

      anchors {
        top: searchBar.bottom
        topMargin: __style.margin20
        left: parent.left
        right: parent.right
        bottom: parent.bottom
      }

      Column {
        width: parent.width
        height: childrenRect.height

        spacing: 0

        MMListSpacer { height: __style.margin20 }

        Column {
          id: invitationsGroup

          width: parent.width
          height: childrenRect.height

          visible: invRepeater.count > 0 && !searchBar.text

          Text {
            text: qsTr( "Pending invitations" )

            width: parent.width
            height: paintedHeight
            elide: Text.ElideRight

            color: __style.nightColor
            font: __style.p6
          }

          MMListSpacer { height: __style.margin6 }

          MMLine {}

          MMListSpacer { height: __style.margin20 }

          Column {
            width: parent.width
            height: childrenRect.height

            spacing: __style.spacing12

            Repeater {
              id: invRepeater

              model: root.invitationsModel

              delegate: MMAccountComponents.MMWorkspaceInvitationDelegate {
                width: parent.width

                text: model.display

                onAccepted: root.invitationClicked( model.whatsThis, true )
                onRejected: root.invitationClicked( model.whatsThis, false )
              }
            }
          }
        }

        MMListSpacer { height: __style.margin20 }

        Column {
          // workspace group

          width: parent.width
          height: childrenRect.height

          Text {
            text: qsTr( "Your workspaces" )

            width: parent.width
            height: paintedHeight
            elide: Text.ElideRight

            color: __style.nightColor
            font: __style.p6
          }

          MMListSpacer { height: __style.margin6 }

          MMLine {}

          MMListSpacer { height: __style.margin20 }

          Column {
            width: parent.width
            height: childrenRect.height

            spacing: __style.spacing12

            Repeater {
              model: root.workspacesModel

              delegate: MMAccountComponents.MMWorkspaceDelegate {
                width: parent.width

                workspaceName: model.display
                isHighlighted: model.whatsThis === root.activeWorkspaceId

                onClicked: root.workspaceClicked( model.whatsThis )
              }
            }
          }
        }

        MMListFooterSpacer {}
      }
    }
  }
}
