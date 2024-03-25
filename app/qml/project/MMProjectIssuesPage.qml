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

import "../components" as MMComponents

MMComponents.MMPage {
  id: root

  property var projectIssuesModel: ListModel {}
  property string projectLoadingLog: ""

  function reportIssue(title, message) {
    projectIssuesModel.append( { title: title, message: message } )
  }

  function clear() {
    projectIssuesModel.clear()
  }

  onBackClicked: root.visible = false
  pageHeader.title: qsTr("Project issues")

  pageContent: ListView {
    id: mainList

    anchors.fill: parent

    model: root.projectIssuesModel
    spacing: __style.margin12

    clip: true

    delegate: Rectangle {
      color: __style.lightGreenColor
      width: ListView.view.width
      height: row.height

      Column {
        id: row

        width: parent.width
        spacing: __style.margin4

        MMComponents.MMText {
          id: nameTextItem

          width: parent.width
          font: __style.t1
          text: title
          wrapMode: Text.Wrap
        }

        MMComponents.MMText {
          id: messageTextItem

          width: parent.width
          text: message
          wrapMode: Text.Wrap
          font: __style.p5
        }
      }
    }

    footer: Column {
      width: ListView.view.width
      spacing: 0

      MMComponents.MMListSpacer { height: __style.margin40 }

      MMComponents.MMText {
        width: mainList.width
        text: qsTr("QGIS log")
        font: __style.t1
      }

      MMComponents.MMListSpacer { height: __style.margin4 }

      MMComponents.MMText {
        width: mainList.width
        text: projectLoadingLog
        wrapMode: Text.Wrap
        font: __style.p5
      }
    }
  }
}
