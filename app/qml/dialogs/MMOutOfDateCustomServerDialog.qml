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
import "../inputs"

MMDrawerDialog {
  id: root

  signal ignoreClicked()

  picture: __style.negativeMMSymbolImage
  bigTitle: qsTr( "Your server will soon be out of date" )
  description: qsTr("Please contact your server administrator to upgrade your server to the latest version. Subsequent releases of our mobile app may not be compatible with your current server version.")
  primaryButton: qsTr( "Learn more about how to migrate" )
  secondaryButton: qsTr( "Ignore" )

  onPrimaryButtonClicked: {
    Qt.openUrlExternally( __inputHelp.migrationGuides )
    close()
  }

  onSecondaryButtonClicked: {
    root.ignoreClicked()
    close()
  }
}
