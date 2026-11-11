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

  signal replaceAudio()

  imageSource: __style.negativeMMSymbolImage
  title: qsTr( "Replace recording?" )
  description: qsTr( "The current recording will be replaced by a new one when you save the feature." )
  primaryButton.text: qsTr( "Record again" )
  secondaryButton.text: qsTr( "Cancel" )

  onPrimaryButtonClicked: {
    root.replaceAudio()
    close()
  }

  onSecondaryButtonClicked: close()
}
