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

  signal migrationRequested()

  picture: __style.positiveMMSymbolImage
  bigTitle: qsTr( "Upload project to Mergin?" )
  description: qsTr( "This project is currently not uploaded on Mergin. Upload it to Mergin in order to activate synchronization and collaboration." )
  primaryButton: qsTr( "Yes, I want to upload it" )
  secondaryButton: qsTr( "No, thanks" )

  onPrimaryButtonClicked: {
    root.migrationRequested()
    close()
  }

  onSecondaryButtonClicked: {
    close()
  }
}
