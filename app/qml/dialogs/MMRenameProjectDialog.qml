/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components"
import "../inputs"

MMDrawer {
  id: root

  property string relatedProjectId: ""
  property string newProjectName: ""
  property string renameErrorText: ""

  signal renameClicked( string newName )

  drawerHeader.title: qsTr( "Rename the project" )
  drawerHeader.titleFont: __style.t2

  drawerContent: Column {
    width: parent.width
    spacing: __style.spacing10

    MMTextInput {
      id: newNameField

      width: parent.width
      textFieldBackground.color: root.renameErrorText === "" ? __style.lightGreenColor : __style.negativeUltraLightColor
      textFieldBackground.border.width: root.renameErrorText === "" ? 0 : __style.width2
      textFieldBackground.border.color: root.renameErrorText === "" ? __style.polarColor : __style.negativeColor

      placeholderText: qsTr( "Enter the new name" )
      text: root.newProjectName

      onTextEdited: root.clearRenameError()
    }

    Item {
      width: parent.width
      height: __style.row24

      visible: true

      Row {
        width: parent.width
        height: parent.height
        anchors.verticalCenter: parent.verticalCenter
        visible: root.renameErrorText !== ""
        spacing: __style.margin4

        MMIcon {
          y: parent.height / 2 - height / 2
          source: __style.errorCircleIcon
          color: __style.negativeColor
          size: __style.icon16
        }

        MMText {
          width: parent.width - __style.icon16 - parent.spacing
          text: root.renameErrorText
          color: __style.grapeColor
          font: __style.t4
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideRight
        }
      }

    }

    MMButton {
      width: parent.width

      text: qsTr( "Confirm" )

      onClicked: {
        root.renameClicked( newNameField.text )
      }
    }
  }

  function clearRenameError() {
    root.renameErrorText = ""
  }

  function showRenameError( message ) {
    root.renameErrorText = message
  }

}