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
import QtQuick.Controls.Basic

import MMInput

import "../../app/qml/components"
import "../../app/qml"
import "../../app/qml/project"
import "../../app/qml/settings"
import "../../app/qml/gps"
import "../../app/qml/account"

Page {
  id: root

  MMStackView {
    id: stackview

    anchors.fill: parent

    initialItem: Page {
      id: pane

      Column {
        width: parent.width
        spacing: 5
        padding: 10

        Button {
          text: "MMProjectLoadingPage"
          onClicked: {
            stackview.push(loadingPageComponent)
          }
        }

        Button {
          text: "MMLogPage"
          onClicked: {
            stackview.push(logPanelComponent)
          }
        }

        Button {
          text: "MMAboutPage"
          onClicked: {
            stackview.push(aboutPanelComponent)
          }
        }

        Button {
          text: "MMChangelogPage"
          onClicked: {
            stackview.push(changelogPanelComponent)
          }
        }

        Button {
          text: "MMSettingsPage"
          onClicked: {
            stackview.push(settingsPanelComponent)
          }
        }

        Button {
          text: "MMProjectIssuesPage"
          onClicked: {
            stackview.push(projectIssuesPageComponent)
          }
        }

        Button {
          text: "MMSwitchWorkspacePage"
          onClicked: {
            stackview.push(switchWorkspacePageComponent)
          }
        }
      }
    }
  }

  Component {
    id: loadingPageComponent

    MMProjectLoadingPage {
      id: loadingScreen

      width: root.width
      height: root.height

      MouseArea {
        width: parent.width
        height: parent.height

        onClicked: {
          stackview.pop()
        }
      }
    }
  }

  Component {
    id: projectIssuesPageComponent

    MMProjectIssuesPage {

      id: projectIssuesPage

      width: root.width
      height: root.height

      projectIssuesModel: ListModel {
          ListElement { title: "Apple" ; message: "2.45" }
          ListElement { title: "ipsum" ; message: "Lorem ipsum dolor sit amet, consectetur adipiscing elit" }
          ListElement { title: "Lorem" ; message: "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exerci" }
          ListElement { title: "sit" ; message: "dunt ut labore et dolore magna aliqua." }
          ListElement { title: "amet" ; message: "adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo" }
          ListElement { title: "consectetur adipiscing" ; message: "amet" }
          ListElement { title: "Apple" ; message: "2.45" }
          ListElement { title: "ipsum" ; message: "Lorem ipsum dolor sit amet, consectetur adipiscing elit" }
          ListElement { title: "Lorem" ; message: "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exerci" }
          ListElement { title: "sit" ; message: "dunt ut labore et dolore magna aliqua." }
          ListElement { title: "amet" ; message: "adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo" }
          ListElement { title: "consectetur adipiscing" ; message: "amet" }
      }

      projectLoadingLog: "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

      MouseArea {
        width: parent.width
        height: parent.height

        onClicked: {
          stackview.pop()
        }
      }
    }
  }

  Component {
    id: logPanelComponent

    MMLogPage {
      id: logPanel
      width: root.width
      height: root.height
      submitReportPending: false
      text: __logText
      onSubmitReport: submitReportPending = !submitReportPending
      onBackClicked: stackview.pop()
    }
  }

  Component {
    id: aboutPanelComponent
    MMAboutPage {
      id: aboutPanel
      width: root.width
      height: root.height
      onVisitWebsiteClicked: Qt.openUrlExternally( "https://merginmaps.com" )
      onBackClicked: stackview.pop()
    }
  }

  Component {
    id: changelogPanelComponent

    MMChangelogPage {
      id: changelogPanel
      width: root.width
      height: root.height
      onBackClicked: stackview.pop()
      dataNotReady: false

      property date today: new Date()

      model: ListModel {
        ListElement {
          date: "Mon, 21 August 2023"
          description: "I am pleased to announce that position tracking has been released today as part of Mobile version 2.3.0 and Plugin version 2023.3"
          title: "Position tracking is now available"
          link: "https://wishlist.merginmaps.com/changelog"
        }

        ListElement {
          date: "Mon, 21 August 2023"
          description: "Mergin Maps QGIS plugin is now capable of setting a custom QGIS."
          title: "Ability to set custom name for photos taken in Mergin Maps"
          link: "https://wishlist.merginmaps.com/changelog"
        }

        function seeChangelogs() {
          console.log("see changelogs requested")
        }
      }
    }
  }

  Component {
    id: settingsPanelComponent

    MMSettingsPage {
      id: settingsPanel
      width: root.width
      height: root.height
      onBackClicked: stackview.pop()
      onManageGpsClicked: console.log("onManageGpsClicked clicked")
      onAboutClicked: console.log("onAboutClicked clicked")
      onChangelogClicked: console.log("onChangelogClicked clicked")
      onHelpClicked: console.log("onHelpClicked clicked")
      onPrivacyPolicyClicked: console.log("onPrivacyPolicyClicked clicked")
      onTermsOfServiceClicked: console.log("onTermsOfServiceClicked clicked")
      onDiagnosticLogClicked: console.log("onDiagnosticLogClicked clicked")
    }
  }

  Component {
    id: switchWorkspacePageComponent

    MMSwitchWorkspacePage {
      id: switchWorkspacePanel

      activeWorkspaceId: 111

      invitationsModel: ListModel {
        ListElement { display: "Workspace-ABC"; whatsThis: "aaa-111-bbb" }
        ListElement { display: "Workspace-EFG"; whatsThis: "aaa-111-bbb" }
      }

      workspacesModel: ListModel {
        ListElement { display: "Workspace-123"; whatsThis: "aaa-111-ccc" }
        ListElement { display: "Workspace-456"; whatsThis: "aaa-111-ddd" }
      }

      onBackClicked: { console.log("BACK") }
      onCreateWorkspaceRequested: { console.log("CREATE WS") }
    }
  }
}
