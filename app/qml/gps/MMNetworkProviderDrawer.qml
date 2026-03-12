/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components" as MMComponents
import "../inputs" as MMInputs

MMComponents.MMDrawer {
  id: root

  signal confirmed( string alias, string deviceAddress )

  drawerHeader.title: qsTr( "Network connection" )
  drawerHeader.titleFont: __style.t2

  drawerContent: Column {
    width: parent.width
    spacing: __style.spacing20

    MMComponents.MMText {
      width: parent.width

      text: qsTr( "External receivers can be connected via network in iOS devices. Some of the known devices that support network connection (TCP or UDP) are EOS and Emlid." )

      font: __style.p5
      color: __style.nightColor

      wrapMode: Text.Wrap
      horizontalAlignment: Text.AlignJustify
    }

    MMInputs.MMTextInput {
      id: ipAddressInput

      width: parent.width
      textFieldBackground.color: __style.lightGreenColor

      title: qsTr( "IP address" )
      placeholderText: qsTr( "localhost" )

      onTextEdited: errorMsg = ""
    }

    MMInputs.MMTextInput {
      id: portInput

      width: parent.width
      textFieldBackground.color: __style.lightGreenColor

      title: qsTr( "Port" )
      placeholderText: qsTr( "1234" )

      textField.inputMethodHints: Qt.ImhDigitsOnly

      onTextEdited: errorMsg = ""
    }

    MMInputs.MMTextInput {
      id: aliasInput

      width: parent.width
      textFieldBackground.color: __style.lightGreenColor

      title: qsTr( "Receiver nickname (optional)" )
      placeholderText: qsTr( "Green device" )
    }

    MMComponents.MMButton {
      width: parent.width

      text: qsTr( "Confirm" )

      onClicked: {
        const ip = ipAddressInput.text.trim()
        const port = portInput.text.trim()

        const ipv4Regex = /^((25[0-5]|2[0-4]\d|1\d{2}|[1-9]\d|\d)\.){3}(25[0-5]|2[0-4]\d|1\d{2}|[1-9]\d|\d)$/
        const hostnameRegex = /^[a-zA-Z0-9]([a-zA-Z0-9\-\.]*[a-zA-Z0-9])?$/

        if ( ip === "" ) {
          ipAddressInput.errorMsg = qsTr( "IP address is required" )
        }
        else if ( !ipv4Regex.test( ip ) && !hostnameRegex.test( ip ) ) {
          ipAddressInput.errorMsg = qsTr( "Enter a valid IP address or hostname" )
        }
        else {
          ipAddressInput.errorMsg = ""
        }

        const portNum = parseInt( port )
        if ( port === "" ) {
          portInput.errorMsg = qsTr( "Port is required" )
        }
        else if ( !/^\d+$/.test( port ) || portNum < 1 || portNum > 65535 ) {
          portInput.errorMsg = qsTr( "Enter a valid port (1–65535)" )
        }
        else {
          portInput.errorMsg = ""
        }

        if ( ipAddressInput.errorMsg !== "" || portInput.errorMsg !== "" ) {
          return
        }

        const deviceAddress = ip + ":" + port

        root.confirmed( aliasInput.text, deviceAddress )
        root.close()
      }
    }
  }
}
