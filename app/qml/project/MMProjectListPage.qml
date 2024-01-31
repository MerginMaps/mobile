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

import notificationType 1.0

Page {
  id: root

  property string headerTitle: qsTr("Projects")
  property bool withSearch: false

  signal backClicked

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  MMHeader {
    id: header

    title: root.headerTitle
    onBackClicked: root.backClicked()
  }

  Column {
    width: parent.width - 40 * __dp
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: header.bottom
    anchors.topMargin: 10 * __dp
    spacing: 20 * __dp

    MMSearchEditor {
      id: searchBar

      width: parent.width - 2 * root.padding
      placeholderText: qsTr("Search for projects...")
      visible: root.withSearch

      onSearchTextChanged: function(text) {
        //root.model.searchExpression = text
      }
    }

    ListView {
      id: listView

      bottomMargin: primaryButton.visible ? primaryButton.height + 20 * __dp : 0
      width: parent.width - 2 * root.padding
      height: ApplicationWindow.window ? ApplicationWindow.window.height - searchBar.height - 150 * __dp : 0
      clip: true
      spacing: 10 * __dp

      model: 10

      delegate: MMProjectItem {
        width: listView.width

        highlight: false
        projectId: "1"
        projectStatus: 2
        projectDisplayName: "Mergin local project"
        projectDescription: "Highlighted"
        projectIsValid: true
        projectIsLocal: true
        projectIsMergin: true
        projectIsPending: false

        onOpenRequested: console.log("onOpenRequested")
        onStopSyncRequested: projectIsPending = false
        onShowChangesRequested: console.log("onShowChangesRequested")
        onSyncRequested: projectIsPending = true
        onRemoveRequested: console.log("onRemoveRequested")
        onMigrateRequested: console.log("onMigrateRequested")
      }
    }
  }
}

