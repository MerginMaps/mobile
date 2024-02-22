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
import Qt5Compat.GraphicalEffects

import lc 1.0
import ".."
import "../components"

Page {
  id: root

  property var projectIssuesModel: ListModel {}
  property string projectLoadingLog: ""

  function reportIssue(title, message) {
    projectIssuesModel.append( { title: title, message: message } )
  }

  function clear() {
    projectIssuesModel.clear()
  }

  Keys.onReleased: function (event) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      root.visible = false
    }
  }

  header: MMHeader {
    id: header

    width: parent.width
    onBackClicked: root.visible = false

    title: qsTr("Project issues")
    titleFont: __style.t3
  }

  background: Rectangle { color: __style.lightGreenColor }

  ListView {
    id: mainList

    anchors.fill: parent
    anchors.margins: __style.pageMargins

    model: root.projectIssuesModel
    spacing: __style.margin12

    delegate: Rectangle {
      color: __style.lightGreenColor
      width: ListView.view.width
      height: row.height

      Column {
        id: row

        width: parent.width
        anchors.left: parent.left
        anchors.top: parent.top

        spacing: __style.margin4

        Text {
          id: nameTextItem
          width: parent.width
          font: __style.t1
          text: title
          color: __style.forestColor
          wrapMode: Text.Wrap
        }

        Text {
          id: messageTextItem
          width: parent.width
          text: message
          wrapMode: Text.Wrap
          font: __style.p5
          color:  __style.nightColor
        }
      }
    }

    footer: Item {
      Item {
        id: spacer

        width: mainList.width
        height: __style.margin40
      }

      Text {
        id: qgisLogTextHeader

        anchors.top: spacer.bottom

        width: mainList.width
        text: qsTr("QGIS log")
        font: __style.t1
        color:  __style.forestColor
      }

      Text {
        id: qgisLogTextItem

        anchors.top: qgisLogTextHeader.bottom
        anchors.topMargin: __style.margin4

        width: mainList.width
        text: projectLoadingLog
        wrapMode: Text.Wrap
        font: __style.p5
        color:  __style.nightColor
      }
    }
  }
}
