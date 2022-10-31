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

  title: qsTr( "We could not split the feature" )
  text: qsTr( "Please make sure that the split line crosses your feature. The feature needs to have a valid geometry in order to split it." )

  buttons: MessageDialog.Ok
}
