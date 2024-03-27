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

  imageSource: __style.positiveMMSymbolImage
  title: qsTr( "Upload project?" )
  description: qsTr( "This project is currently not uploaded on cloud. Upload it in order to activate synchronization and collaboration." )
  primaryButton.text: qsTr( "Yes, I want to upload it" )
  secondaryButton.text: qsTr( "No, thanks" )

  onPrimaryButtonClicked: {
    root.migrationRequested()
    close()
  }

  onSecondaryButtonClicked: {
    close()
  }
}
