/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Dialogs 1.3

MessageDialog {
  id: root

  signal singInRequested()

  title: qsTr( "Sign in to Mergin" )
  text: qsTr( "You need to be signed in to your Mergin account in order to synchronize a project. Hit 'Yes' to sign in." )

  standardButtons: StandardButton.No | StandardButton.Yes

  onButtonClicked: {
    if ( clickedButton === StandardButton.Yes ) {
      root.singInRequested()
    }
  }
}
