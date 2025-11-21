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

  imageSource: __style.negativeMMSymbolImage
  title: qsTr( "We could not split the feature" )
  description: qsTr( "Ensure that your split line intersects the feature properly and that the feature’s geometry is valid." )
  primaryButton.text: qsTr( "Ok, I understand" )

  onPrimaryButtonClicked: close()

}
