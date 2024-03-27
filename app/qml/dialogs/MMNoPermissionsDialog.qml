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

  imageSource: __style.noPermissionsImage
  title: qsTr( "No rights to access the project" )
  description: qsTr( "You are not allowed to synchronize your changes in this project. Contact the project owner to assign you the correct permission. If you are the project owner, log in to the dashboard." )
  primaryButton.text: qsTr( "Ok, I understand" )

  onPrimaryButtonClicked: {
    close()
  }
}
