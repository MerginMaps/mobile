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

  signal deleteAudio()
  signal unlinkAudio()

  property string audioPath
  property bool offerUnlinkOption: true

  imageSource: __style.negativeMMSymbolImage
  title: qsTr( "Delete audio?" )
  description: root.offerUnlinkOption ? qsTr( "Would you like to delete or unlink the audio? Deleting removes the file from your project entirely, while unlinking keeps the file in your project but removes it from this specific feature." ) : qsTr( "Would you like to delete the audio?" )

  primaryButton {
    text: qsTr( "Delete audio" )

    fontColor: __style.grapeColor
    bgndColor: __style.negativeColor
    fontColorHover: __style.negativeColor
    bgndColorHover: __style.grapeColor
  }

  secondaryButton {
    text: qsTr( "Unlink audio" )

    visible: root.offerUnlinkOption

    fontColor: __style.grapeColor
    fontColorHover: __style.negativeColor
  }

  onPrimaryButtonClicked: root.deleteAudio()
  onSecondaryButtonClicked: root.unlinkAudio()
}
