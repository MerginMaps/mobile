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

import "./components" as MMAccountComponents
import "../components/"
import "../inputs"

MMPage {
  id: root

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

  signal selectWorkspaceClicked
  signal manageAccountClicked
  signal closeAccountClicked
  signal signOutClicked

  pageBottomMarginPolicy: MMPage.BottomMarginPolicy.PaintBehindSystemBar

  pageContent: MMScrollView {

    width: parent.width
    height: parent.height

    ColumnLayout {

      width: parent.width
      height: parent.height

      spacing: 0

      MMAvatarLettersItem {

        abbrv: root.abbrName

        Layout.preferredWidth: width
        Layout.preferredHeight: height

        Layout.alignment: Qt.AlignHCenter
      }

      Text {
        Layout.fillWidth: true
        Layout.topMargin: __style.margin12

        maximumLineCount: 2
        elide: Text.ElideRight
        wrapMode: Text.WrapAnywhere

        text: root.fullName ? root.fullName + " (" + root.userName + ")" : root.userName
        color: __style.nightColor
        font: __style.t1

        horizontalAlignment: Text.AlignHCenter
      }

      Text {
        Layout.fillWidth: true
        Layout.topMargin: __style.margin12

        maximumLineCount: 2
        elide: Text.ElideRight
        wrapMode: Text.WrapAnywhere

        text: root.email
        color: __style.nightColor
        font: __style.p5

        horizontalAlignment: Text.AlignHCenter
      }

      Text {
        Layout.fillWidth: true
        Layout.topMargin: __style.margin40

        text: qsTr("Workspaces")
        color: __style.nightColor
        font: __style.p6
      }

      MMLine {
        width: parent.width
      }

      MMAccountComponents.MMAccountPageItem {

        Layout.fillWidth: true
        Layout.preferredHeight: height
        Layout.topMargin: __style.margin20

        title: root.workspaceName
        desc: root.workspaceRole
        iconSource: __style.workspacesIcon
        notificationCount: root.invitationsCount

        onClicked: root.selectWorkspaceClicked()
      }

      MMAccountComponents.MMAccountPageItem {

        Layout.fillWidth: true
        Layout.preferredHeight: height
        Layout.topMargin: __style.margin20

        title: qsTr("Manage Account")
        desc: root.subscription
        iconSource: __style.settingsIcon

        onClicked: root.manageAccountClicked()
      }

      Item {
        Layout.fillWidth: true
        Layout.preferredHeight: progressBar.height + storageTitleText.height + __style.margin6
        Layout.topMargin: __style.margin20
        Layout.rightMargin: __style.margin2

        MMText {
          id: storageTitleText

          anchors.left: parent.left
          text: qsTr("Storage")
          font: __style.p6
        }

        MMText {
          anchors.right: parent.right
          text: root.storage
          color: __style.forestColor
          font: __style.t4
        }


        MMProgressBar {
          id: progressBar

          anchors.bottom: parent.bottom
          anchors.left: parent.left
          anchors.right: parent.right

          position: root.storageFill
          color: position > 0.9 ? __style.negativeColor : __style.grassColor
          progressColor: position > 0.9 ? __style.grapeColor : __style.forestColor
        }
      }

      Text {
        Layout.fillWidth: true
        Layout.topMargin: __style.margin40

        text: qsTr("General")
        color: __style.nightColor
        font: __style.p6
      }

      MMLine {
        width: parent.width
      }

      MMAccountComponents.MMAccountPageItem {

        Layout.fillWidth: true
        Layout.preferredHeight: height
        Layout.topMargin: __style.margin20

        title: qsTr("Sign out")
        iconSource: __style.signOutIcon

        onClicked: root.signOutClicked()
      }

      MMAccountComponents.MMAccountPageItem {

        Layout.fillWidth: true
        Layout.preferredHeight: height
        Layout.topMargin: __style.margin20

        title: qsTr("Close account")
        desc: qsTr("This will delete all your data")
        iconSource: __style.closeAccountIcon

        onClicked: root.closeAccountClicked()
      }

      MMListFooterSpacer {}
    }
  }
}
