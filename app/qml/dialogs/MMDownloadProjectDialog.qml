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

  imageSource: __style.positiveMMSymbolImage
  title: qsTr( "Download project" )
  description: qsTr( "Would you like to download project\n %1 ?" ).arg( relatedProjectId )
  primaryButton.text: qsTr("Download")
  secondaryButton.text: qsTr("Cancel")

  onPrimaryButtonClicked: {
    downloadClicked()
    close()
  }

  onSecondaryButtonClicked: {
    root.relatedProjectId = ""
    close()
  }
}
