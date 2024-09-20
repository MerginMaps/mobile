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

  imageSource: __style.signInImage
  title: qsTr( "Sign in to your account" )
  description: qsTr( "You need to be signed in to your account in order to synchronise the project." )
  primaryButton.text: qsTr( "Yes, I want to sign in" )
  secondaryButton.text: qsTr( "No, thanks" )

  onPrimaryButtonClicked: {
    root.singInRequested()
    close()
  }

  onSecondaryButtonClicked: {
    close()
  }
}
