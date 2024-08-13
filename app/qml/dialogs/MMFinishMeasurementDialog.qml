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

  signal finishMeasurementRequested()

  imageSource: __style.neutralMMSymbolImage
  title: qsTr( "Do you  wish to finish the measurement?" )
  description: qsTr( "Your measured segment will be lost." )
  primaryButton.text: qsTr( "Yes" )
  secondaryButton.text: qsTr( "No" )

  onPrimaryButtonClicked: {
    console.log(" Measurement : Finish YES")
    root.finishMeasurementRequested()
    close()
  }

  onSecondaryButtonClicked: {
    console.log(" Measurement : Finish NO")
    close()
  }
}
