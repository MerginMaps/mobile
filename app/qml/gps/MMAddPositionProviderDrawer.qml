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

import lc 1.0

import "../components"

Drawer {
  id: root

  height: view.height + header.height + borderRectangle.height + discoveryInProgressItem.height
  width: window.width
  edge: Qt.BottomEdge
  focus: true
  dim: true
  interactive: false
  dragMargin: 0
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

  signal close
  signal initiatedConnectionTo( string deviceAddress, string deviceName )

  Component.onCompleted: {
    forceActiveFocus()
  }

  BluetoothPermission {
    id: btPermission

    communicationModes: BluetoothPermission.Access

    onStatusChanged: btModel.initiateBtDiscovery()
  }

  Keys.onReleased: function( event ) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      btModel.discovering = false
      close()
    }
  }


  Rectangle {
    id: borderRectangle

    color: roundedRect.color
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: 2 * radius
    anchors.topMargin: -radius
    radius: 20 * __dp
  }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: __style.whiteColor

    MMPageHeader {
      id: header

      title: qsTr( "Connect to bluetooth device" )
      backVisible: false

      MMRoundButton {
        id: backBtn

        anchors.right: parent.right
        anchors.rightMargin: __style.pageMargins
        anchors.verticalCenter: parent.verticalCenter

        iconSource: __style.closeIcon
        iconColor: __style.forestColor

        bgndColor: __style.lightGreenColor
        bgndHoverColor: __style.mediumGreenColor

        onClicked: {
          btModel.discovering = false
          root.close()
        }
      }
    }

    Column {
      id: discoveryInProgressItem

      anchors.bottom: parent.bottom
      anchors.left: parent.left
      anchors.leftMargin: __style.pageMargins
      anchors.bottomMargin: __style.pageMargins
      width: parent.width - 2 * __style.pageMargins
      spacing: __style.margin12
      visible: btModel.discovering

      Image {
        anchors.horizontalCenter: parent.horizontalCenter
        id: loadingSpinner
        source: __style.mmSymbolImage
        sourceSize.width: __style.icon32
      }

      Text {
        anchors.horizontalCenter: parent.horizontalCenter

        text: view.count > 2 ? qsTr("Looking for more devices") + " ..." : qsTr("Looking for devices") + " ..."
        font: __style.t3
        color: __style.forestColor
        y: parent.height / 2 - contentHeight / 2

        wrapMode: Text.WordWrap
        elide: Text.ElideRight
      }
    }

    ListView {
      id: view

      width: parent.width
      anchors.left: parent.left
      anchors.leftMargin: __style.pageMargins
      anchors.top: header.top
      anchors.topMargin: __style.margin40

      spacing: __style.margin12

      model: BluetoothDiscoveryModel {
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
            __inputUtils.showNotification( qsTr( "Bluetooth permission is required in order to connect to external receivers. Please enable it in system settings" ) )
          }
          else
          {
            btPermission.request()
          }
        }
      }

      Component.onCompleted: btModel.initiateBtDiscovery()

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
            __inputUtils.showNotification( qsTr( "You need to enable Bluetooth in order to connect new GPS receiver" ) )
            root.close()
          }
        }
      }

      delegate: Rectangle {
        id: providerDelegate

        width: ListView.view.width
        height: __style.row49

        Row {
          id: row

          anchors.fill: parent
          anchors.leftMargin: __style.pageMargins
          anchors.rightMargin: __style.pageMargins
          anchors.bottomMargin: 5 * __dp
          anchors.topMargin: 5 * __dp

          Column {
            width: row.width - connectIconContainer.width
            height: row.height

            Text {
              id: deviceName

              width: parent.width
              height: parent.height * 0.5

              text: model.DeviceName ? model.DeviceName : qsTr( "Unknown device" )

              elide: Text.ElideRight
              color: __style.nightColor
              font: __style.t3
            }

            Text {
              id: deviceAddress

              width: parent.width
              height: parent.height * 0.5

              text: model.DeviceAddress

              elide: Text.ElideRight
              color: __style.nightColor
              font: __style.p6
            }
          }

          MMRoundButton {
            id: connectIconContainer

            height: parent.height
            width: parent.height

            bgndColor: __style.lightGreenColor
            iconSource: __style.plusIcon
          }
        }

        MouseArea {
          anchors.fill: parent
          onClicked: {
            __positionKit.positionProvider = __positionKit.constructProvider( "external", model.DeviceAddress, model.DeviceName )
            root.initiatedConnectionTo( model.DeviceAddress, model.DeviceName )

            connectionDialogLoader.active = true
            connectionDialogLoader.focus = true
          }
        }

        Rectangle {
          width: providerDelegate.width / 1.5
          height: 2 * __dp

          color: __style.lightGreenColor
          visible: index < providerDelegate.ListView.view.count - 1

          anchors.top: row.bottom
          anchors.horizontalCenter: parent.horizontalCenter
        }
      }
    }

    Loader {
      id: connectionDialogLoader

      sourceComponent: connectionDialogComponent
      active: false
      asynchronous: true
      anchors.fill: parent

      onLoaded: item.open()
    }

    Component {
      id: connectionDialogComponent

      MMBluetoothConnectionDrawer {
        id: connectionDialog

        width: root.width * 0.8
        height: root.height / 2

        anchors.centerIn: parent

        onSuccess: {
          btModel.discovering = false
          root.close()
        }

        onFailure: {
          // keep discovering, revert position provider back to internal provider
          __positionKit.positionProvider = __positionKit.constructProvider( "internal", "devicegps", "" )
        }

        onClosed: dialogLoader.active = false
      }
    }
  }
}
