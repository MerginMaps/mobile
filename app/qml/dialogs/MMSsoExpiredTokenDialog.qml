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

  imageSource: __style.noPermissionsImage
  title: qsTr( "SSO login has expired" )
  description: qsTr( "Your SSO login has expired. To access your remote projects and be able to synchronize, you need to log in again." )
  primaryButton.text: qsTr( "I want to log in" )
  secondaryButton.text: qsTr( "I'll log in later" )

  onPrimaryButtonClicked: {
    root.singInRequested()
    close()
  }

  onSecondaryButtonClicked: {
    close()
  }
}
