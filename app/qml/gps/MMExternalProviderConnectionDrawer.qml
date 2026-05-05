/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import mm 1.0 as MM
import MMInput

import "../components" as MMComponents

MMComponents.MMDrawer {
  id: root

  property string providerType: ""
  property var positionProvider: PositionKit.positionProvider

  property string connectingSuffixAnimation: ""

  signal success()
  signal failure()

  StateGroup {
    id: rootstate

    states: [
      State {
        name: "working"
        when: root.positionProvider && root.positionProvider.state === MM.PositionProvider.Connecting
        PropertyChanges {
          target: message
          image: root.providerType === "bluetooth" ? __style.externalBluetoothGreenImage : __style.externalNetworkGreenImage
          title: root.providerType === "network"
            ? qsTr( "Connecting to external receiver" )
            : ( root.positionProvider.name()
                ? qsTr( "Connecting to" ) + " " + root.positionProvider.name()
                : qsTr( "Connecting" ) + root.connectingSuffixAnimation )
          description: root.providerType === "bluetooth"
            ? qsTr( "You might be asked to pair your device during this process." )
            : ( root.positionProvider.getIpAddress()
                ? qsTr( "Connecting to" ) + " " + root.positionProvider.getIpAddress() + qsTr( ". You can close this panel, the app will continue in the background." )
                : qsTr( "Connecting" ) + root.connectingSuffixAnimation )
          linkText: ""
        }
      },
      State {
        name: "success"
        when: root.positionProvider && root.positionProvider.state === MM.PositionProvider.Connected
        PropertyChanges {
          target: message
          image: root.providerType === "bluetooth" ? __style.externalBluetoothGreenImage : __style.externalNetworkGreenImage
          title: qsTr( "Connected" )
          description: ""
          linkText: ""
        }
      },
      State {
        name: "fail"
        when: !root.positionProvider || root.positionProvider.state === MM.PositionProvider.NoConnection
        PropertyChanges {
          target: message
          image: __style.externalGpsRedImage
          title: qsTr( "Failed to connect to" ) + " " + ( root.positionProvider
            ? ( root.providerType === "network" ? root.positionProvider.getIpAddress() : root.positionProvider.name() )
            : "" )
          description: root.providerType === "bluetooth"
            ? qsTr( "We were not able to connect to the specified device. Please make sure your device is powered on and can be connected to." )
            : qsTr( "We were not able to connect to the specified IP address. Please try again later." )
          linkText: qsTr( "Learn more" )
        }
      },
      State {
        name: "waitingToReconnect"
        when: root.positionProvider && root.positionProvider.state === MM.PositionProvider.WaitingToReconnect
        PropertyChanges {
          target: message
          image: root.providerType === "bluetooth" ? __style.externalBluetoothGreenImage : __style.externalNetworkGreenImage
          title: root.providerType === "bluetooth"
            ? qsTr( "We were not able to connect to the specified device. Please make sure your device is powered on and can be connected to." )
            : qsTr( "We were not able to connect to the specified IP address. Please try again later." )
          description: root.positionProvider.stateMessage + "<br><br>" + qsTr( "You can close this message, we will try to repeatedly connect to your device." )
          linkText: qsTr( "Learn more" )
        }
      }
    ]

    state: "working"
  }

  drawerBottomMargin: __style.margin40
  drawerContent: MMComponents.MMScrollView {

    width: parent.width
    height: root.maxHeightHit ? root.drawerContentAvailableHeight : contentHeight

    MMComponents.MMMessage {
      id: message

      width: parent.width
      link: __inputHelp.howToConnectGPSLink
    }
  }

  Timer {
    id: closeTimer

    interval: 1500
    repeat: false
    running: rootstate.state === "success"
    onTriggered: root.close()
  }

  Timer {
    // connecting animator
    interval: 400

    repeat: true
    running: rootstate.state === "working"

    onTriggered: {
      if ( root.connectingSuffixAnimation.length > 2 ) {
        root.connectingSuffixAnimation = ""
      }
      else {
        root.connectingSuffixAnimation += "."
      }
    }
  }
}
