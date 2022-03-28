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

  title: qsTr( "No rights to access the project" )
  text: qsTr( "You are not allowed to synchronize this project. Contact your Mergin administrator and let him know you need an access. If you are an administrator, log in to your Mergin dashboard." )

  standardButtons: StandardButton.Ok
}
