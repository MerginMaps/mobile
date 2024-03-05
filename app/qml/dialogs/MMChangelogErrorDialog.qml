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

/**
  * Dialog bearing information about synchronization failure.
  * By default it has some text, but it is also possible to set "detailedText"
  * "detailedText" will be visible either under "details" button or appended to
  * the default text (based on platform).
  */

MMDrawerDialog {
  id: root

  property string detailedText

  picture: __style.negativeMMSymbolImage
  bigTitle: qsTr( "Failed to load changelog" )
  primaryButton: qsTr( "Ok, I understand" )
  boundedDescription: detailedText

  onPrimaryButtonClicked: {
    close()
  }
}
