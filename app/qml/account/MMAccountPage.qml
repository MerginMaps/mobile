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

import "./components"
import "../components"
import "../inputs"

Page {
  id: root

  signal backClicked
  signal selectWorkspaceClicked
  signal manageAccountClicked
  signal closeAccountClicked
  signal signOutClicked

  required property string abbrName
  required property string fullName
  required property string userName
  required property string workspaceName
  required property string workspaceRole
  required property string email
  required property string subscription
  required property string storage
  required property real storageFill // [0-1]
  required property int invitationsCount

  header: MMPageHeader {
    id: header
    color: __style.lightGreenColor
    backVisible: true
    onBackClicked: root.backClicked()
  }

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  Item {
    id: bodyItem
    width: Math.min(parent.width - 2 * __style.pageMargins, __style.maxPageWidth)
    anchors.horizontalCenter: parent.horizontalCenter
    x: __style.pageMargins

    Column {
      id: infoPanel

      anchors.horizontalCenter: parent.horizontalCenter
      spacing: 8 * __dp
      Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        width: 80 * __dp
        height: width
        radius: width / 2
        color: __style.fieldColor

        Text {
          text: root.abbrName
          anchors.fill: parent
          color: __style.forestColor
          font: __style.h2
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
        }
      }

      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        text: root.fullName + " (" + root.userName + ")"
        color: __style.nightColor
        font: __style.t1
      }

      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        text: root.email
        color: __style.nightColor
        font: __style.p5
      }
    }

    Column {
      id: workspacePanel
      anchors.top: infoPanel.bottom
      anchors.topMargin: __style.pageMargins
      width: parent.width
      spacing: 8 * __dp

      Text {
        text: qsTr("Workspaces")
        color: __style.nightColor
        font: __style.t1
      }

      MMLine {
        width: parent.width
      }

      MMAccountPageItem {
        width: parent.width
        title: root.workspaceName
        desc: root.workspaceRole
        iconSource: __style.workspacesIcon
        notificationCount: root.invitationsCount
        onLinkClicked: root.selectWorkspaceClicked()
      }

      MMAccountPageItem {
        width: parent.width
        title: qsTr("Manage Account")
        desc: root.subscription
        iconSource: __style.settingsIcon
        onLinkClicked: root.manageAccountClicked()
      }

      Item {
        width: parent.width
        height: progressBar.height + storageText.height + 8 * __dp

        Text {
          id: storateLabel
          anchors.top: parent.top
          anchors.left: parent.left
          text: qsTr("Storage")
          color: __style.nightColor
          font: __style.p4
        }

        Text {
          id: storageText
          anchors.top: parent.top
          anchors.right: parent.right
          text: root.storage
          color: __style.forestColor
          font: __style.t4
        }

        MMProgressBar {
          id: progressBar
          anchors.bottom: parent.bottom
          anchors.left: parent.left
          width: parent.width
          position: root.storageFill
          color: __style.grassColor
          progressColor: __style.forestColor
        }
      }
    }

    Column {
      id: generalPanel
      width: bodyItem.width
      anchors.top: workspacePanel.bottom
      anchors.topMargin: __style.pageMargins
      spacing: 8 * __dp

      Text {
        text: qsTr("General")
        color: __style.nightColor
        font: __style.t1
      }

      MMLine {
        width: parent.width
      }

      MMAccountPageItem {
        width: parent.width
        title: qsTr("Sign out")
        iconSource: __style.signOutIcon
        onLinkClicked: root.signOutClicked()
      }

      MMAccountPageItem {
        width: parent.width
        title: qsTr("Close account")
        desc: qsTr("This will delete all your data")
        iconSource: __style.closeAccountIcon
        onLinkClicked: root.closeAccountClicked()
      }
    }
  }
}
