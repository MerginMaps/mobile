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
            aboutPanel.visible = true
          }
        }

/*
        MMButton {
          text: "MMChangelogPanel"
          onClicked: {
            changelogPanel.visible = true
          }
        }
        */
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
      visible: false
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
      width: parent.width
      height: parent.height
      visible: false

      MouseArea {
        width: parent.width
        height: parent.height
        onClicked: aboutPanel.visible = false
      }
    }
  }

  /*
  Component {
    id: changelogPanelComponent
    MMChangelogPanel {
      id: changelogPanel
      width: parent.width
      height: parent.height
      visible: false

      MouseArea {
        width: parent.width
        height: parent.height
        onClicked: changelogPanel.visible = false
      }
    }
  }

*/
}
