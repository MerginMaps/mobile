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

  property string projectName: "" // current name, prefilled when the drawer opens
  property alias errorText: newNameField.errorMsg

  signal renameClicked( string newName )
  signal textEdited( string text )

  drawerHeader.title: qsTr( "Rename project" )
  drawerHeader.titleFont: __style.t2

  onAboutToShow: () => {
    newNameField.errorMsg = ""
    newNameField.text = root.projectName
  }

  drawerContent: Column {
    id: contentColumn

    width: parent.width
    spacing: newNameField.errorMsg ? __style.margin12 : __style.spacing40

    MMTextInput {
      id: newNameField

      width: contentColumn.width
      textFieldBackground.color: __style.lightGreenColor

      placeholderText: qsTr( "Enter the new name" )

      // textChanged also covers the clear button, which does not emit textEdited
      onTextChanged: {
        newNameField.errorMsg = newNameField.text.trim() ? "" : qsTr( "The project name cannot be empty" )
        root.textEdited( newNameField.text )
      }
    }

    MMButton {
      width: contentColumn.width

      text: qsTr( "Confirm" )
      enabled: !newNameField.errorMsg

      onClicked: {
        root.renameClicked( newNameField.text )
      }
    }
  }
}