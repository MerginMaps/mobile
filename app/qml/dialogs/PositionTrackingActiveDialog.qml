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

  signal stopRequested()

  title: qsTr( "Do you want to stop position tracking?" )
  text: qsTr( "You can track your position on one project at the time. You need to stop the tracking in order to open another project. Do you want to stop the tracking and open the project?" )

  buttons: MessageDialog.No | MessageDialog.Yes

  onButtonClicked: function(clickedButton) {
    if ( clickedButton === MessageDialog.Yes ) {
      root.stopRequested()
    }
    close()
  }
}
