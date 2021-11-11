/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.12
import lc 1.0

import "./components"

Item {
  id: root

  property int projectModelType: ProjectsModel.EmptyProjectsModel
  property string activeProjectId: ""
  property bool actionRequired: __merginApi.subscriptionInfo.actionRequired
  property alias list: projectlist

  signal openProjectRequested( string projectId, string projectFilePath )
  signal showLocalChangesRequested( string projectId )

  function refreshProjectsList() {
    searchBar.deactivate()
    projectlist.refreshProjectList( searchBar.text )
  }

  Rectangle {
      id: attentionBanner
      color: '#ff4f4f'
      height: InputStyle.rowHeight
      visible: parent.actionRequired

      anchors {
        top: parent.top
        left: parent.left
        right: parent.right
      }

      TextWithIcon {
          width: parent.width
          fontColor: 'white'
          bgColor: '#ff4f4f'
          iconColor: 'white'
          linkColor: 'white'
          source: InputStyle.exclamationTriangleIcon
          text: qsTr("Your attention is required. Please visit the %1Mergin dashboard%2.")
                    .arg("<a href='" + __inputHelp.merginDashboardLink + "'>")
                    .arg("</a>")

      }

      MouseArea {
        anchors.fill: parent
        onClicked: Qt.openUrlExternally( __inputHelp.merginDashboardLink )
      }
  }



  SearchBar {
    id: searchBar

    anchors {
      top: parent.actionRequired ? attentionBanner.bottom : parent.top
      left: parent.left
      right: parent.right
    }

    allowTimer: true
  }

  ProjectList {
    id: projectlist

    projectModelType: root.projectModelType
    activeProjectId: root.activeProjectId
    searchText: searchBar.text

    anchors {
      left: parent.left
      right: parent.right
      top: searchBar.bottom
      bottom: parent.bottom
    }

    onOpenProjectRequested: root.openProjectRequested( projectId, projectFilePath )
    onShowLocalChangesRequested: root.showLocalChangesRequested( projectId )
  }
}
