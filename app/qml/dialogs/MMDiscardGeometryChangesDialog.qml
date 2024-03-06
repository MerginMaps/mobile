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
import "../inputs"

MMDrawerDialog {
  id: root

  property var state

  signal discardChanges()

  picture: __style.positiveMMSymbolImage
  bigTitle: qsTr( "Discard the changes?" )
  description: {
    if ( root.state === "edit" ) {
      return qsTr( "Clicking ‘Yes’ discards your changes to the geometry. If you would like " +
                  "to save the changes instead, hit ‘No’ and then ‘Done’ in the toolbar." )
    }
    else if ( root.state === "record" || root.state === "recordInLayer" ) {
      return qsTr( "Clicking ‘Yes’ discards your new geometry and no feature will be saved. " +
                  "If you would like to save the geometry instead, hit ‘No’ and then ‘Done’ " +
                  "in the toolbar." )
    }
    return ""
  }

  primaryButton: qsTr ( "Yes, I want to discard" )
  secondaryButton: qsTr ( "No, thanks" )

  onPrimaryButtonClicked: {
    root.discardChanges()
    close()
  }

  onSecondaryButtonClicked: {
    close()
  }
}
