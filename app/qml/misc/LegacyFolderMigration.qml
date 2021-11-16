/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.14
import "../"
import "../components"

Item {
  id: root
  property int numOfCopiedProjects: 0
  property int numOfProjectsToCopy: 0

  MessageDialog {
    id: notEnoughSpaceLeftdialog
    property string neededSpace: ""

    standardButtons: StandardButton.Help | StandardButton.Ignore
    text: qsTr("Your device is running out of space, you need %1 of free space in order to see your projects. Remove some files and come back or click Help to see other ways how to resolve this issue.").arg(notEnoughSpaceLeftdialog.neededSpace)
    title: qsTr("Insufficient space left on device")

    onHelp: Qt.openUrlExternally("https://www.lutraconsulting.co.uk/blog/2021/10/26/input-scoped-storage-update/")
  }
  MessageDialog {
    id: migrationFailureDialog
    icon: StandardIcon.Warning
    standardButtons: StandardButton.Help | StandardButton.Ignore
    text: qsTr("Your device run into a problem during applying an update. You will not be able to see your projects. Click Help to see how to resolve this issue.")
    title: qsTr("An error occured during update")

    onHelp: Qt.openUrlExternally("https://www.lutraconsulting.co.uk/blog/2021/10/26/input-scoped-storage-update/")
  }
  Connections {
    target: __androidUtils

    function onMigrationFinished(success) {
      migrationInProgress.visible = false;
      if (!success) {
        migrationFailureDialog.open();
      }
    }
    function onMigrationProgressed(numOfCopiedProjects) {
      root.numOfCopiedProjects = numOfCopiedProjects;
    }
    function onMigrationStarted(numOfProjectsToCopy) {
      root.numOfProjectsToCopy = numOfProjectsToCopy;
      migrationInProgress.visible = true;
    }
    function onNotEnoughSpaceLeftToMigrate(neededSpace) {
      notEnoughSpaceLeftdialog.neededSpace = neededSpace;
      notEnoughSpaceLeftdialog.open();
    }
  }
  Rectangle {
    id: migrationInProgress
    anchors.fill: parent
    color: InputStyle.clrPanelMain
    visible: false

    MouseArea {
      anchors.fill: parent

      onClicked: {
      } // do not propagate clicks
    }
    ColumnLayout {
      anchors.fill: parent
      spacing: 0

      BusyIndicator {
        Layout.alignment: Qt.AlignCenter
        Layout.preferredHeight: parent.width / 8 // the same as width
        Layout.preferredWidth: parent.width / 8
      }
      Text {
        Layout.alignment: Qt.AlignCenter
        Layout.preferredWidth: parent.width * 0.8
        color: InputStyle.fontColor
        elide: Text.ElideRight
        font.pixelSize: InputStyle.fontPixelSizeNormal
        horizontalAlignment: Text.AlignHCenter
        text: qsTr("We are working on an important update, please do not close the application")
        wrapMode: Text.WordWrap
      }
      Text {
        Layout.alignment: Qt.AlignCenter
        Layout.preferredWidth: parent.width * 0.8
        color: InputStyle.fontColor
        elide: Text.ElideRight
        font.pixelSize: InputStyle.fontPixelSizeNormal
        horizontalAlignment: Text.AlignHCenter
        text: qsTr("%1/%2").arg(root.numOfCopiedProjects).arg(root.numOfProjectsToCopy)
        wrapMode: Text.WordWrap
      }
    }

    Behavior on opacity  {
      NumberAnimation {
        duration: 500
      }
    }
  }
}
