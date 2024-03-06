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

  picture: __style.positiveMMSymbolImage
  bigTitle: qsTr( "Your server will soon be out of date" )
  description: qsTr("Please contact your server administrator to upgrade your server to the latest version. " +
                    "Subsequent releases of our mobile app may not be compatible with your current server version. \n" +
                    "Learn more about <a href='%1' style='color: %2;'>how to migrate</a>.")
                    .arg( __inputHelp.migrationGuides )
                    .arg( __style.forestColor )

  primaryButton: qsTr( "Ignore" )

  onPrimaryButtonClicked: {
    root.ignoreClicked()
    close()
  }
}
