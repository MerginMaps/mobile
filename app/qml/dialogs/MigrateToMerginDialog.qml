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

  signal migrationRequested()

  title: qsTr( "Upload project to Mergin?" )
  text: qsTr( "This project is currently not uploaded on Mergin. Upload it to Mergin in order to activate synchronization and collaboration. \nClick 'Yes' to upload it." )

  buttons: MessageDialog.Ok | MessageDialog.Cancel

  onButtonClicked: function(clickedButton) {
    if ( clickedButton === MessageDialog.Ok ) {
      root.migrationRequested()
    }
  }
}
