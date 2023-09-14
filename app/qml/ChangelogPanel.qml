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
import QtQuick.Dialogs

import "."  // import InputStyle singleton
import "./components"
import lc 1.0

Item {
  id: root

  signal close

  Keys.onReleased: function( event ) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      close()
    }
  }

  Page {
    id: pane

    width: parent.width
    height: parent.height
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    clip: true

    background: Rectangle {
      color: "white"
    }

    header: PanelHeader {
      id: header
      height: InputStyle.rowHeightHeader
      width: parent.width
      color: "white"
      rowHeight: InputStyle.rowHeightHeader
      titleText: qsTr("Change Log")

      onBack: root.close()
      withBackButton: true
    }

    Item {
      id: changelogItem
      anchors.horizontalCenter: parent.horizontalCenter
      width: root.width - InputStyle.panelMargin
      height: parent.height

      Component.onCompleted: changelogView.model.seeChangelogs()

      Text {
        id: title
        text: qsTr("What's new")
        wrapMode: Text.WordWrap
        width: parent.width
        font.pixelSize: InputStyle.fontPixelSizeHeader
        color: InputStyle.fontColor
      }

      Text {
        id: subTitle
        anchors.top: title.bottom
        text: qsTr("See what changed since you were last here")
        wrapMode: Text.WordWrap
        width: parent.width
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor
      }

      Button {
        id: closeButton
        anchors.right: parent.right
        onClicked: close()
        contentItem: Text { text: "❌" }
        background: Item {}
      }

      ListView {
        id: changelogView
        width: parent.width
        anchors.top: subTitle.bottom
        anchors.topMargin: 20
        anchors.bottom: parent.bottom
        spacing: 3
        clip: true
        model: ChangelogModel {}
        delegate: MouseArea {
          width: changeItem.width
          height: changeItem.height
          onClicked: Qt.openUrlExternally(link)
          Column {
            id: changeItem
            width: changelogView.width
            Rectangle { width: parent.width; height: 2; color: "lightGray" }
            Text { text: date; font.italic: true; wrapMode: Text.WordWrap; width: parent.width; font.pixelSize: InputStyle.fontPixelSizeNormal; color: InputStyle.fontColor }
            Text { text: title; font.bold: true; wrapMode: Text.WordWrap; width: parent.width; font.pixelSize: InputStyle.fontPixelSizeBig; color: InputStyle.fontColor }
            Text { text: description; wrapMode: Text.WordWrap; width: parent.width; font.pixelSize: InputStyle.fontPixelSizeNormal; color: InputStyle.fontColor }
          }
        }

        ScrollBar.vertical: ScrollBar {
          parent: changelogView.parent
          anchors.top: changelogView.top
          anchors.left: changelogView.right
          anchors.bottom: changelogView.bottom
        }
      }

      Button {
        id: seeAllChangesButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        onClicked: {
          changelogView.model.seeChangelogs(true)
        }
        text: qsTr("Show all changes")
        visible: false
      }
    }

    footer: DelegateButton {
      id: showAllButton

      width: root.width
      height: InputStyle.rowHeightHeader
      text: qsTr("Show all changes")

      onClicked: {
        changelogView.model.seeChangelogs()
      }
    }
  }
}
