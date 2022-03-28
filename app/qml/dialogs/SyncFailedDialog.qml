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

  title: qsTr( "Failed to synchronize your changes" )
  text: qsTr( "Your changes could not be sent to server, make sure you are connected to internet and have write access to this project." )

  standardButtons: StandardButton.Ok

  // It is also possible to set "detailedText"
 // detailedText: ""
}
