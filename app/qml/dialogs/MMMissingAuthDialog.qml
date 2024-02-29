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

  signal singInRequested()

  picture: __style.signInImage
  bigTitle: qsTr( "Sign in to Mergin" )
  description: qsTr( "You need to be signed in to your Mergin Maps account in order to synchronize the project." )
  primaryButton: qsTr( "Yes, I want to sign in" )
  secondaryButton: qsTr( "No, thanks" )

  onPrimaryButtonClicked: {
    root.singInRequested()
    close()
  }

  onSecondaryButtonClicked: {
    close()
  }
}
