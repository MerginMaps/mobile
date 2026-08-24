/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../components"
import "../../inputs"

MMDrawer {
  id: root

  property string errorText: ""

  signal renameClicked( string newName )

  drawerHeader.title: qsTr( "Rename project" )
  drawerHeader.titleFont: __style.t2

  onOpened: {
    root.errorText = ""
    newNameField.text = ""
  }

  drawerContent: Column {
    width: parent.width
    spacing: 0

    MMTextInput {
      id: newNameField

      width: parent.width
      textFieldBackground.color: root.errorText === "" ? __style.lightGreenColor : __style.negativeUltraLightColor
      textFieldBackground.border.width: root.errorText === "" ? 0 : __style.width2
      textFieldBackground.border.color: root.errorText === "" ? __style.polarColor : __style.negativeColor

      placeholderText: qsTr( "Enter the new name" )

      onTextEdited: root.errorText = ""
    }

    // Fixed-height slot so the drawer does not grow/shrink when the error message appears.
    Item {
      width: parent.width
      height: __style.spacing40

      Row {
        anchors.verticalCenter: parent.verticalCenter

        width: parent.width
        spacing: __style.margin4

        visible: root.errorText !== ""

        MMIcon {
          y: parent.height / 2 - height / 2
          source: __style.errorCircleIcon
          color: __style.negativeColor
          size: __style.icon16
        }

        MMText {
          width: parent.width - __style.icon16 - parent.spacing
          text: root.errorText
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
}