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

  signal signInRequested()

  imageSource: __style.noPermissionsImage
  title: qsTr( "Your session has expired" )
  description: qsTr( "Your session has expired, please log in again." )
  primaryButton.text: qsTr( "I want to log in" )
  secondaryButton.text: qsTr( "I'll log in later" )

  onPrimaryButtonClicked: {
    root.signInRequested()
    close()
  }

  onSecondaryButtonClicked: {
    close()
  }
}
