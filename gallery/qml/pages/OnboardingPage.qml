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

import "../../app/qml/onboarding"

Page {
  id: pane

  Component {
      id: onboarding
      MMOnboarding {
          anchors.fill: parent

          onCloseOnboarding: {
              loader.active = false
          }
      }
  }

  ColumnLayout {
    id: layout

    Label {
      text: "Onboarding (Login, Sign Up)"
    }

    Label {
      text: "Username: test"
    }

    Label {
      text: "Password: test"
    }

    ToolButton {
      onClicked: {
        console.log("Onboarding started")
        loader.active = true
      }
      text: "Start Here (Login)!"
    }
  }

  Loader {
      id: loader
      sourceComponent: onboarding
      anchors.fill: parent
      active: false
  }
}
