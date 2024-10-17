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

  imageSource: __style.positiveMMSymbolImage
  title: qsTr( "New Look & Feel" )
  description: qsTr( "We've been busy making the app even better! This update brings a fresh look and improved navigation, making it faster to find what you need. Take a look around!" )
  primaryButton.text: qsTr("Let's start!")

  onPrimaryButtonClicked: {
    close()
  }
}
