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

  picture: __style.negativeMMSymbolImage
  bigTitle: qsTr( "Saving failed" )
  description: qsTr( "Failed to save changes. This should not happen normally. Please restart the app and try again — if that does not help, please contact support." )
  primaryButton: qsTr( "Ok, I understand" )

  onPrimaryButtonClicked: {
    close()
  }
}
