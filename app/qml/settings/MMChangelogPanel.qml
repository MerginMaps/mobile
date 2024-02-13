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

import "../components"

import lc 1.0

Item {
  id: root

  signal close

  required property var model /* ChangelogModel */

  Keys.onReleased: function( event ) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      close()
    }
  }

  MessageDialog {
    id: errorDialog

    title: qsTr( "Failed to load changelog" )
    buttons: MessageDialog.Ok
    onButtonClicked: {
      errorDialog.close()
      root.close()
    }
  }

  Page {
    width: parent.width
    height: parent.height
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    clip: true

    background: Rectangle {
      color: __style.lightGreenColor
    }

    header: MMHeader {
      id: header
      title: qsTr("Changelog")
      titleFont: __style.t3

      onBackClicked: root.close()
      backVisible: true
    }

    Item {
      anchors.horizontalCenter: parent.horizontalCenter
      width: root.width - 2 * __style.pageMargins
      height: parent.height

      Component.onCompleted: changelogView.model.seeChangelogs()

      Text {
        id: subTitle

        anchors.top: title.bottom
        text: qsTr("What's new")
        wrapMode: Text.WordWrap
        width: parent.width
        font: __style.t1
        color: __style.forestColor
      }

      Text {
        id: description

        anchors.top: subTitle.bottom
        anchors.topMargin: 10 * __dp
        text: qsTr("See what changed since you were last here.")
        wrapMode: Text.WordWrap
        width: parent.width
        font: __style.p5
        color: __style.nightColor
      }

      ListView {
        id: changelogView

        width: parent.width
        anchors.top: description.bottom
        anchors.topMargin: __style.pageMargins
        anchors.bottom: parent.bottom
        spacing: __style.pageMargins
        clip: true
        model: root.model

        delegate: MouseArea {
          width: changeItem.width
          height: changeItem.height
          onClicked: Qt.openUrlExternally(model.link)

          Column {
            id: changeItem
            width: changelogView.width
            spacing: 10 * __dp

            MMLine {}

            Text {
              text: model.title
              wrapMode: Text.WordWrap
              width: parent.width
              font: __style.t1
              color: __style.nightColor
            }

            Text {
              // TODO move date formatting to c++
              text: model.date instanceof Date ? Qt.locale().dayName( model.date.getDay(), Locale.ShortFormat ) + ", " + model.date.getDate() + " " + Qt.locale().monthName( model.date.getMonth(), Locale.LongFormat ) : model.date
              wrapMode: Text.WordWrap
              width: parent.width
              font: __style.p6
              color: __style.forestColor
            }

            Text {
              text: model.description
              wrapMode: Text.WordWrap
              width: parent.width
              font: __style.p5
              color: __style.nightColor
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
  }
}
