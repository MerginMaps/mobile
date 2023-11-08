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

  MessageDialog {
    id: errorDialog

    title: qsTr( "Getting What's new failed" )
    buttons: MessageDialog.Ok
    onButtonClicked: close()
  }

  Page {
    width: parent.width
    height: parent.height
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    clip: true

    background: Rectangle {
      color: InputStyle.panelBackgroundWhite
    }

    header: PanelHeader {
      height: InputStyle.rowHeightHeader
      width: parent.width
      color: InputStyle.clrPanelMain
      rowHeight: InputStyle.rowHeightHeader
      titleText: qsTr("What's new")

      onBack: root.close()
      withBackButton: true
    }

    Item {
      anchors.horizontalCenter: parent.horizontalCenter
      width: root.width - InputStyle.panelMargin
      height: parent.height

      Component.onCompleted: changelogView.model.seeChangelogs()

      Text {
        id: subTitle

        anchors.top: title.bottom
        text: qsTr("See what changed since you were last here")
        wrapMode: Text.WordWrap
        width: parent.width
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor
      }

      ListView {
        id: changelogView

        width: parent.width
        anchors.top: subTitle.bottom
        anchors.topMargin: InputStyle.panelMargin
        anchors.bottom: parent.bottom
        spacing: InputStyle.panelMargin
        clip: true
        model: ChangelogModel {
          onErrorMsgChanged: function(msg) {
            errorDialog.text = msg
            errorDialog.open()
          }
        }

        delegate: MouseArea {
          width: changeItem.width
          height: changeItem.height
          onClicked: Qt.openUrlExternally(model.link)

          Column {
            id: changeItem
            width: changelogView.width

            Rectangle {
              width: parent.width
              height: InputStyle.changelogLineWidth
              color: InputStyle.changelogLineWColor
            }
            Text {
              text: Qt.locale().dayName( model.date.getDay(), Locale.ShortFormat ) + ", " + model.date.getDate() + " " + Qt.locale().monthName( model.date.getMonth(), Locale.LongFormat )
              font.italic: true
              wrapMode: Text.WordWrap
              width: parent.width
              font.pixelSize: InputStyle.fontPixelSizeNormal
              color: InputStyle.fontColor
            }
            Text {
              text: model.title
              font.bold: true;
              wrapMode: Text.WordWrap
              width: parent.width
              font.pixelSize: InputStyle.fontPixelSizeBig
              color: InputStyle.fontColor
            }
            Text {
              text: description
              wrapMode: Text.WordWrap
              width: parent.width
              font.pixelSize: InputStyle.fontPixelSizeNormal
              color: InputStyle.fontColor
            }
          }
        }

        ScrollBar.vertical: ScrollBar {
          parent: changelogView.parent
          anchors.top: changelogView.top
          anchors.left: changelogView.right
          anchors.bottom: changelogView.bottom
        }
      }
    }

    footer: DelegateButton {
      id: refreshButton

      width: root.width
      height: InputStyle.rowHeightHeader
      text: qsTr("Refresh")
      visible: !changelogView.count

      onClicked: {
        changelogView.model.seeChangelogs()
      }
    }
  }
}
