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

MMDrawerDialog {
  id: root

  signal saveChanges()
  signal discardChanges()

  picture: __style.positiveMMSymbolImage
  bigTitle: qsTr( "Unsaved changes" )
  description: qsTr( "Do you want to save the changes?" )
  primaryButton: qsTr( "Yes, I want to save" )
  secondaryButton: qsTr( "No, thanks" )

  onPrimaryButtonClicked: {
    root.saveChanges()
    close()
  }

  onSecondaryButtonClicked: {
    root.discardChanges()
    close()
  }
}
