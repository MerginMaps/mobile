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

  signal deleteImage()
  signal unlinkImage()

  property string imagePath
  property bool offerUnlinkOption: true
  property bool signatureMode: false

  imageSource: __style.negativeMMSymbolImage
  title: root.signatureMode ? qsTr( "Delete signature?" ) : qsTr( "Delete photo?" )
  description: {
    if ( root.signatureMode )
      return qsTr( "Deleting removes the signature from this feature entirely. The feature will need to be signed again." )

    if ( root.offerUnlinkOption )
      return qsTr( "Would you like to delete or unlink the photo? Deleting removes the photo from your project entirely, while unlinking keeps the photo in your project but removes it from this specific feature." )

    return qsTr( "Would you like to delete the photo?" )
  }

  primaryButton {
    text: root.signatureMode ? qsTr( "Delete signature" ) : qsTr( "Delete photo" )

    fontColor: __style.grapeColor
    bgndColor: __style.negativeColor
    fontColorHover: __style.negativeColor
    bgndColorHover: __style.grapeColor
  }

  secondaryButton {
    text: qsTr( "Unlink photo" )

    visible: root.offerUnlinkOption

    fontColor: __style.grapeColor
    fontColorHover: __style.negativeColor
  }

  onPrimaryButtonClicked: root.deleteImage()
  onSecondaryButtonClicked: root.unlinkImage()
}
