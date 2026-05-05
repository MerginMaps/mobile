/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtCore
import QtQuick
import QtQuick.Controls
import mm 1.0 as MM

import "../components" as MMComponents

MMComponents.MMListDrawer {
  id: root

  signal initiatedConnectionTo( string deviceAddress, string deviceName )

  showFullScreen: true
  drawerHeader.title: qsTr( "Bluetooth Connection" )

  list.height: root.drawerContentAvailableHeight

  list.model: MM.BluetoothDiscoveryModel {
    id: btModel

    discovering: false

    function initiateBtDiscovery() {
      // Is bluetooth permission granted and is bluetooth turned on?
      // For Android we need to opt to enable Bluetooth and listen on response in the connections component.

      if ( btPermission.status === Qt.Granted )
      {
        if ( __inputUtils.isBluetoothTurnedOn() )
        {
          btModel.discovering = true
        }
        else
        {
          __inputUtils.turnBluetoothOn()
        }
      }
      else if ( btPermission.status === Qt.Denied )
      {
        __notificationModel.addInfo( qsTr( "Bluetooth permission is required in order to connect to external receivers. Please enable it in system settings" ) )
      }
      else
      {
        btPermission.request()
      }
    }
  }

  list.delegate: MMComponents.MMListDelegate {
    text: model.DeviceName ? model.DeviceName : qsTr("Unknown device")
    secondaryText: model.DeviceAddress

    rightContent: MMComponents.MMRoundButton {
      bgndColor: __style.lightGreenColor
      iconSource: __style.plusIcon

      onClicked: root.initiatedConnectionTo( model.DeviceAddress, model.DeviceName )
    }

    onClicked: root.initiatedConnectionTo( model.DeviceAddress, model.DeviceName )
  }

  list.footer: btModel.discovering ? discoveringMessageComponent : null

  Component.onCompleted: btModel.initiateBtDiscovery()

  BluetoothPermission {
    id: btPermission

    communicationModes: BluetoothPermission.Access

    onStatusChanged: btModel.initiateBtDiscovery()
  }

  Connections {
    target: __androidUtils

    function onBluetoothEnabled( state )
    {
      if ( state )
      {
        btModel.discovering = true
      }
      else
      {
        __notificationModel.addInfo( qsTr( "You need to enable Bluetooth in order to connect new GPS receiver" ) )
        root.close()
      }
    }
  }

  // footer
  Component {
    id: discoveringMessageComponent

    Item {
      width: ListView.view.width
      height: Math.max( contentColumn.implicitHeight,
                        ListView.view.height - ( ListView.view.contentHeight - height ) )

      Column {
        id: contentColumn

        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        width: parent.width

        Image {
          anchors.horizontalCenter: parent.horizontalCenter

          source: __style.mmSymbolImage

          width: __style.icon32 * __dp
          height: __style.icon32 * __dp

          fillMode: Image.PreserveAspectFit
        }

        MMComponents.MMListSpacer { height: __style.margin16 }

        MMComponents.MMText {
          width: parent.width

          text: qsTr( "Scanning for devices" ) + "..."

          font: __style.t3
          color: __style.forestColor

          horizontalAlignment: Text.AlignHCenter
        }
        MMComponents.MMListSpacer { height: __style.margin16 }
      }
    }
  }
}
