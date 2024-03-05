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

MMDrawerDialog {
  id: root

  signal okButtonClicked()

  picture: __style.negativeMMSymbolImage
  bigTitle: qsTr( "Failed to load changelog" )
  primaryButton: qsTr( "Ok, I understand" )

  onPrimaryButtonClicked: {
    root.okButtonClicked()
    close()
  }
}
