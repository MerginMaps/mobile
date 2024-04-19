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

import "../../inputs" as MMInputs
import "../../components" as MMComponents

MMSettingsItem {
  id: root

  property string valueDescription

  onClicked: {
    drawerLoader.active = true
  }

  Loader {
    id: drawerLoader

    active: false
    asynchronous: true

    sourceComponent: MMComponents.MMDrawer {
      id: drawer

      drawerHeader.title: root.title

      onClosed: drawerLoader.active = false

      drawerContent: MMComponents.MMScrollView {

        width: parent.width
        height: drawer.maxHeightHit ? drawer.drawerContentAvailableHeight : contentHeight

        Column {
          width: parent.width
          spacing: 0

          MMComponents.MMListSpacer { height: __style.spacing20 }

          MMInputs.MMTextInput {
            id: textInput

            width: parent.width

            title: root.title

            textFieldBackground.color: __style.lightGreenColor

            text: root.value
            textField.inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
          }

          MMComponents.MMListSpacer { height: __style.spacing40 }

          MMComponents.MMButton {
            width: parent.width

            text: qsTr( "Confirm" )

            onClicked: {
              root.valueWasChanged( textInput.text )
              drawer.close()
            }
          }
        }

        Component.onCompleted: open()
      }
    }
  }
}



