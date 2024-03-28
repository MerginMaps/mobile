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
  signal keepImage()

  property string imagePath

  imageSource: __style.negativeMMSymbolImage
  title: qsTr( "Remove photo reference" )
  description: qsTr( "Also permanently delete photo from device?" )
  primaryButton.text: qsTr( "Yes, I want to delete" )
  secondaryButton.text: qsTr( "No, thanks" )

  onPrimaryButtonClicked: {
    root.deleteImage()
    close()
  }

  onSecondaryButtonClicked: {
    root.keepImage()
    close()
  }
}
