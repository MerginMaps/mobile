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

  property var positionProvider: PositionKit.positionProvider
  property string howToConnectGPSLink: __inputHelp.howToConnectGPSLink

  property string titleText: {
    if ( rootstate.state === "working" )
    {
      if ( !root.positionProvider ) return ""
      if ( root.positionProvider.name() ) return qsTr( "Connecting to" ) + " " + root.positionProvider.name() + connectingSuffixAnimation
      return qsTr( "Connecting" ) + connectingSuffixAnimation
    }
    else if ( rootstate.state === "success" )
    {
      return qsTr( "Connected" )
    }
    else
    {
      // either NoConnection or WaitingToReconnect
      return qsTr( "Failed to connect to" ) + " " + ( root.positionProvider ? root.positionProvider.name() : "" )
    }
  }

  property string connectingSuffixAnimation: ""

  property string descriptionText: {
    if ( rootstate.state === "working" )
    {
      return qsTr( "You might be asked to pair your device during this process." )
    }
    else if ( rootstate.state === "success" )
    {
      return ""
    }
    else if ( rootstate.state === "waitingToReconnect" )
    {
      return PositionKit.positionProvider.stateMessage + "<br><br>" +
          qsTr( "You can close this message, we will try to repeatedly connect to your device." )
    }

    else
    {
      return qsTr( "We were not able to connect to the specified device. Please make sure your device is powered on and can be connected to." )
    }
  }

  property var imageSource: {
    if ( rootstate.state === "fail" )
    {
      return __style.externalGpsRedImage
    }
    else {
      return __style.externalGpsGreenImage
    }
  }

  signal success()
  signal failure()

  StateGroup {
    id: rootstate

    states: [
      State {
        name: "working"
        when: root.positionProvider && root.positionProvider.state === MM.PositionProvider.Connecting
      },
      State {
        name: "success"
        when: root.positionProvider && root.positionProvider.state === MM.PositionProvider.Connected
      },
      State {
        name: "fail"
        when: !root.positionProvider || root.positionProvider.state === MM.PositionProvider.NoConnection
      },
      State {
        name: "waitingToReconnect"
        when: !root.positionProvider || root.positionProvider.state === MM.PositionProvider.WaitingToReconnect
      }
    ]

    state: "working"
  }

  drawerBottomMargin: __style.margin20
  drawerContent: MMComponents.MMScrollView {

    width: parent.width
    height: root.maxHeightHit ? root.drawerContentAvailableHeight : contentHeight

    MMComponents.MMMessage {
      width: parent.width

      image: root.imageSource
      title: root.titleText
      description: root.descriptionText
      link: root.howToConnectGPSLink
      linkText: ( rootstate.state === "working" || rootstate.state === "success" ) ? "" : qsTr( "Learn more" )
    }
  }

  Timer {
    id: closeTimer

    interval: 1500
    repeat: false
    running: rootstate.state === "success"
    onTriggered: {
      root.close()
    }
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
