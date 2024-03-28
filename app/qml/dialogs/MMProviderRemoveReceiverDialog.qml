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

  signal removeProvider()

  property string providerId

  imageSource: __style.positiveMMSymbolImage
  title: qsTr( "Remove receiver" )
  description: qsTr( "Do you want to remove receiver from the list of recent receivers?" )
  primaryButton.text: qsTr( "Yes, I want to remove" )
  secondaryButton.text: qsTr( "No, thanks" )

  onPrimaryButtonClicked: {
    root.removeProvider()
    close()
  }

  onSecondaryButtonClicked: {
    providerId = ""
    close()
  }
}
