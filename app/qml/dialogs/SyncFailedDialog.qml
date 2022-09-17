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

/**
  * Dialog bearing information about synchronization failure.
  * By default it has some text, but it is also possible to set "detailedText"
  * "detailedText" will be visible either under "details" button or appended to
  * the default text (based on platform).
  */
MessageDialog {
  id: root

  title: qsTr( "Failed to synchronize your changes" )
  text: qsTr( "Your changes could not be sent to server, make sure you are connected to internet and have write access to this project." )

  buttons: StandardButton.Ok

//  detailedText: ""
}
