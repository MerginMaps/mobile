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

  signal backClicked
  signal selectWorkspaceClicked
  signal manageAccountClicked
  signal closeAccountClicked
  signal signOutClicked

  property string abbrName: "PB"
  property string fullName: "Patrik Bell"
  property string userName: "Chuck Norris"
  property string workspaceRole: "owner"
  property string email: "patrik.bell@merginmaps.com"
  property string subscription: "Professional v2 (10GB)"
  property string storage: "120.6 MB / 1.8 GB"
  property real storageFill: 0.2

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  header: MMHeader {
    id: header
    backVisible: true
    onBackClicked: root.backClicked()
  }

  Item {
    id: bodyItem
    width: Math.min(parent.width - 2 * __style.pageMargins, __style.maxPageWidth)

    Column {
      id: infoPanel
      width: parent.width
      spacing: 8 * __dp
      Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        width: 80 * __dp
        height: width
        radius: width / 2
        color: __style.grassColor

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
      padding: __style.pageMargins
      anchors.top: infoPanel.bottom
      anchors.topMargin: 2 * __style.pageMargins
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

      MMAcountPageItem {
        width: parent.width
        title: root.fullName
        desc: root.workspaceRole
        iconSource: __style.workspacesIcon
        notificationCount: 1
        onLinkClicked: root.selectWorkspaceClicked()
      }

      MMAcountPageItem {
        width: parent.width
        title: qsTr("Manage Account")
        desc: root.subscription
        iconSource: __style.settingsIcon
        onLinkClicked: root.manageAccountClicked()
      }

      // TODO storage progress bar
    }

    Column {
      id: generalPanel
      padding: __style.pageMargins
      width: bodyItem.width
      anchors.top: workspacePanel.bottom
      anchors.topMargin: 2 * __style.pageMargins
      spacing: 8 * __dp

      Text {
        text: qsTr("General")
        color: __style.nightColor
        font: __style.t1
      }

      MMLine {
        width: parent.width
      }

      MMAcountPageItem {
        width: parent.width
        title: qsTr("Sign out")
        desc: root.workspaceRole
        iconSource: __style.signOutIcon
        onLinkClicked: root.signOutClicked()
      }

      MMAcountPageItem {
        width: parent.width
        title: qsTr("Close account")
        desc: qsTr("This will delete all your data")
        iconSource: __style.closeAccountIcon
        onLinkClicked: root.closeAccountClicked()
      }
    }
  }
}
