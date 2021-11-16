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

  property int numOfProjectsToCopy: 0
  property int numOfCopiedProjects: 0

  MessageDialog {
    id: notEnoughSpaceLeftdialog

    property string neededSpace: ""

    title: qsTr( "Insufficient space left on device" )

    text: qsTr( "Your device is running out of space, you need %1 of free space in order to see your projects. Remove some files and come back or click Help to see other ways how to resolve this issue." ).arg( notEnoughSpaceLeftdialog.neededSpace )
    standardButtons: StandardButton.Help | StandardButton.Ignore

    onHelp: Qt.openUrlExternally( "https://www.lutraconsulting.co.uk/blog/2021/10/26/input-scoped-storage-update/" )
  }

  MessageDialog {
    id: migrationFailureDialog

    title: qsTr( "An error occured during update" )
    icon: StandardIcon.Warning

    text: qsTr( "Your device run into a problem during applying an update. You will not be able to see your projects. Click Help to see how to resolve this issue." )
    standardButtons: StandardButton.Help | StandardButton.Ignore

    onHelp: Qt.openUrlExternally( "https://www.lutraconsulting.co.uk/blog/2021/10/26/input-scoped-storage-update/" )
  }

  Connections {
    target: __androidUtils

    function onMigrationStarted( numOfProjectsToCopy ) {
      root.numOfProjectsToCopy = numOfProjectsToCopy
      migrationInProgress.visible = true
    }

    function onMigrationProgressed( numOfCopiedProjects ) {
      root.numOfCopiedProjects = numOfCopiedProjects
    }

    function onMigrationFinished( success ) {
      migrationInProgress.visible = false

      if ( !success )
      {
        migrationFailureDialog.open()
      }
    }

    function onNotEnoughSpaceLeftToMigrate( neededSpace ) {
      notEnoughSpaceLeftdialog.neededSpace = neededSpace
      notEnoughSpaceLeftdialog.open()
    }
  }

  Rectangle {
    id: migrationInProgress

    anchors.fill: parent
    color: InputStyle.clrPanelMain

    visible: false

    MouseArea {
      anchors.fill: parent
      onClicked: {} // do not propagate clicks
    }

    ColumnLayout {
      anchors.fill: parent

      spacing: 0

      BusyIndicator {

        Layout.alignment: Qt.AlignCenter

        Layout.preferredWidth: parent.width / 8
        Layout.preferredHeight: parent.width / 8 // the same as width
      }

      Text {
        text: qsTr( "We are working on an important update, please do not close the application" )

        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor

        Layout.alignment: Qt.AlignCenter
        Layout.preferredWidth: parent.width * 0.8

        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
        wrapMode: Text.WordWrap
      }

      Text {
        text: qsTr( "%1/%2" ).arg( root.numOfCopiedProjects ).arg( root.numOfProjectsToCopy )

        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor

        Layout.alignment: Qt.AlignCenter
        Layout.preferredWidth: parent.width * 0.8

        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
        wrapMode: Text.WordWrap
      }
    }

    Behavior on opacity {
      NumberAnimation{ duration: 500 }
    }
  }
}
