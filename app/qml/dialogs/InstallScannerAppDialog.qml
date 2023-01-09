/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Dialogs

MessageDialog {
  id: root

  signal installRequested()

  text: qsTr( "Install scanner application" )
  informativeText: qsTr( "You will be redirected to Google Play to install the Barcode Scanner app." )

  buttons: MessageDialog.Ok

  onButtonClicked: function ( button, role ) {
    switch ( button ) {
    case MessageDialog.Ok:
      root.installRequested()
      close()
      break;
    }
  }
}
