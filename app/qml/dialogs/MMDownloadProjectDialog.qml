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

import "../components"
import "../inputs"

MMDrawerDialog {
  id: root

  property string relatedProjectId: ""

  signal downloadClicked()

  picture: __style.positiveMMSymbolImage
  bigTitle: qsTr( "Download project" )
  description: qsTr( "Would you like to download the project\n %1 ?" ).arg( relatedProjectId )
  primaryButton: qsTr("Download")
  secondaryButton: qsTr("Cancel")

  onPrimaryButtonClicked: {
    downloadClicked()
    close()
  }

  onSecondaryButtonClicked: {
    root.relatedProjectId = ""
    close()
  }
}
