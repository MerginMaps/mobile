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
  title: qsTr( "Microphone access needed" )
  description: qsTr( "Mergin Maps needs access to the microphone to record audio for your features. You can allow it in the system settings." )

  // iOS is the only platform where the app settings can be opened by a URL
  primaryButton.text: __iosUtils.isIos ? qsTr( "Open settings" ) : qsTr( "Ok, I understand" )
  secondaryButton.text: qsTr( "Cancel" )
  secondaryButton.visible: __iosUtils.isIos

  onPrimaryButtonClicked: {
    if ( __iosUtils.isIos ) {
      Qt.openUrlExternally( "app-settings:" )
    }
    close()
  }

  onSecondaryButtonClicked: close()
}
