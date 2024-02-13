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

import "../../app/qml/components"
import "../../app/qml"
import "../../app/qml/project"
import "../../app/qml/settings"

Item {
  id: root
  anchors.fill: parent

  StackView {
    id: stackview

    anchors.fill: parent

    initialItem: Page {
      id: pane

      Column {
        width: parent.width
        spacing: 10
        padding: 10

        Label {
          text: "Pages"
        }

        MMButton {
          text: "MMProjectLoadingScreen"
          onClicked: {
            stackview.push(loadingScreenComponent)
          }
        }

        MMButton {
          text: "MMLogPanel"
          onClicked: {
            stackview.push(logPanelComponent)
          }
        }

        MMButton {
          text: "MMAboutPanel"
          onClicked: {
            stackview.push(aboutPanelComponent)
          }
        }

        MMButton {
          text: "MMChangelogPanel"
          onClicked: {
            stackview.push(changelogPanelComponent)
          }
        }
      }
    }
  }

  Component {
    id: loadingScreenComponent
    MMProjectLoadingScreen {
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
    id: logPanelComponent

    MMLogPanel {
      id: logPanel
      width: root.width
      height: root.height
      submitReportPending: false
      text: __logText
      onSubmitReport: submitReportPending = !submitReportPending
      onClose: stackview.pop()
    }
  }

  Component {
    id: aboutPanelComponent
    MMAboutPanel {
      id: aboutPanel
      width: root.width
      height: root.height
      onVisitWebsiteClicked: Qt.openUrlExternally( "https://merginmaps.com" )
      onClose: stackview.pop()
    }
  }

  Component {
    id: changelogPanelComponent
    MMChangelogPanel {
      id: changelogPanel
      width: root.width
      height: root.height
      onClose: stackview.pop()

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
}
