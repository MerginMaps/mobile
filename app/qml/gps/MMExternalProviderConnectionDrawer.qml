/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQml

import MMInput

import "../components" as MMComponents

MMComponents.MMDrawer {
  id: root

  property string providerType: ""
  property PositionProvider positionProvider: PositionKit.positionProvider

  property string connectingSuffixAnimation: ""

  signal success()
  signal failure()

  StateGroup {
    id: rootstate

    states: [
      State {
        name: "connecting"
        when: root.positionProvider && root.positionProvider.state === PositionProvider.Connecting
        PropertyChanges {
          message.image: root.getExternalProviderImage()
          message.title: root.getConnectingTitle()
          message.description: root.getConnectingDescription()
          message.linkText: ""
        }
      },
      State {
        name: "success"
        when: root.positionProvider && root.positionProvider.state === PositionProvider.Connected
        PropertyChanges {
          message.image: root.getExternalProviderImage()
          message.title: qsTr( "Connected" )
          message.description: ""
          message.linkText: ""
        }
      },
      State {
        name: "fail"
        when: !root.positionProvider || root.positionProvider.state === PositionProvider.NoConnection
        PropertyChanges {
          message.image: __style.externalGpsRedImage
          message.title: root.getFailTitle()
          message.description: root.getFailDescription()
          message.linkText: qsTr( "Learn more" )
        }
      },
      State {
        name: "waitingToReconnect"
        when: root.positionProvider && root.positionProvider.state === PositionProvider.WaitingToReconnect
        PropertyChanges {
          message.image: __style.externalGpsRedImage
          message.title: root.getWaitingToReconnectTitle()
          message.description: qsTr( "%1%2You can close this message, we will try to repeatedly connect to your device." ).arg( root.positionProvider.stateMessage ).arg( "<br><br>" )
          message.linkText: qsTr( "Learn more" )
        }
      }
    ]

    state: "connecting"
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
    running: rootstate.state === "connecting"

    onTriggered: {
      if ( root.connectingSuffixAnimation.length > 2 ) {
        root.connectingSuffixAnimation = ""
      }
      else {
        root.connectingSuffixAnimation += "."
      }
    }
  }

  function getExternalProviderImage() {
    switch ( root.providerType ) {
      case "bluetooth":
        return __style.externalBluetoothGreenImage
      case "network":
        return __style.externalNetworkGreenImage
      case "trimble":
        return __style.externalNetworkGreenImage
      default:
        return ""
    }
  }

  function getConnectingTitle() {
    if ( root.providerType === "network" ) {
      return qsTr( "Connecting to external receiver" )
    }
    else {
      if ( root.positionProvider.name() ) {
        return qsTr( "Connecting to %1" ).arg( root.positionProvider.name() )
      }
      else {
        return qsTr( "Connecting%1" ).arg( root.connectingSuffixAnimation )
      }
    }
  }

  function getConnectingDescription() {
    if ( root.providerType === "bluetooth" ) {
      return qsTr( "You might be asked to pair your device during this process." )
    }
    else {
      if ( root.positionProvider.id() && root.providerType !== "trimble" ) {
        return qsTr( "Connecting to %1. You can close this panel, the app will continue in the background." ).arg( root.positionProvider.id() )
      }
      else if ( root.providerType === "trimble" ) {
        return qsTr( "Trimble Mobile Manager will be opened shortly." )
      }
      else {
        return qsTr( "Connecting%1" ).arg( root.connectingSuffixAnimation )
      }
    }
  }

  function getFailTitle() {
    if ( root.providerType === "trimble"  || !root.positionProvider ) {
      return qsTr( "Failed to connect" )
    }
    else {
      let providerName
      if ( root.providerType === "network" ) {
        providerName = root.positionProvider.id()
      }
      else {
        providerName = root.positionProvider.name()
      }

      return  qsTr( "Failed to connect to %1" ).arg( providerName )
    }
  }

  function getFailDescription() {
    if ( root.providerType === "bluetooth" ) {
      return qsTr( "We were not able to connect to the specified device. Please make sure your device is powered on and can be connected to." )
    }
    else if ( root.providerType === "network" ) {
      return qsTr( "We were not able to connect to the specified IP address." )
    }
    else if ( root.providerType === "trimble" ) {
      return qsTr( "We were not able to connect to Trimble Mobile Manager. Please make sure it's installed." )
    }
  }

  function getWaitingToReconnectTitle() {
    return getFailDescription()
  }
}
