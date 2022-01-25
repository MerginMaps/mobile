/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

import lc 1.0

import "../" // import InputStyle singleton
import "../components" as Components

Page {
  id: root

  signal close
  signal initiatedConnectionTo( string deviceAddress, string deviceName )

  header: Components.PanelHeader {
    id: header

    height: InputStyle.rowHeightHeader
    width: parent.width
    color: InputStyle.clrPanelMain
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr( "Connect to bluetooth device" )

    onBack: {
      btModel.discovering = false
      root.close()
    }

    withBackButton: true
  }

  focus: true

  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      btModel.discovering = false
      close()
    }
  }

  ListView {
    id: view

    anchors.fill: parent

    model: BluetoothDiscoveryModel {
      id: btModel
      discovering: false
    }

    Component.onCompleted: {
      // Is bluetooth turned on?
      // For Android we need to opt to enable Bluetooth and listen on response in the connections component.
      if ( __inputUtils.isBluetoothTurnedOn() )
      {
        btModel.discovering = true
      }
      else
      {
        __inputUtils.turnBluetoothOn()
      }
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
          __inputUtils.showNotification( qsTr( "You need to enable Bluetooth in order to connect new GPS receiver" ) )
          root.close()
        }
      }
    }

    delegate: Rectangle {
      id: providerDelegate

      width: ListView.view.width
      height: InputStyle.rowHeight

      Row {
        id: row

        anchors.fill: parent
        anchors.leftMargin: InputStyle.panelMargin
        anchors.rightMargin: InputStyle.panelMargin
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
            color: InputStyle.fontColor
            font.pixelSize: InputStyle.fontPixelSizeNormal
          }

          Text {
            id: deviceAddress

            width: parent.width
            height: parent.height * 0.5

            text: model.DeviceAddress

            elide: Text.ElideRight
            color: InputStyle.secondaryFontColor
            font.pixelSize: InputStyle.fontPixelSizeSmall
          }
        }

        Item {
          id: connectIconContainer

          height: parent.height
          width: parent.height

          Image {
            id: connectIcon

            anchors.centerIn: parent

            width: parent.height / 2
            sourceSize.width: parent.height / 2

            source: InputStyle.plusIcon
          }

          ColorOverlay {
            anchors.fill: connectIcon
            source: connectIcon
            color: InputStyle.darkGreen
          }
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

        color: InputStyle.panelBackgroundLight
        visible: index < providerDelegate.ListView.view.count - 1

        anchors.top: row.bottom
        anchors.horizontalCenter: parent.horizontalCenter
      }
    }

    footer: Item {
      id: footerDelegate

      width: ListView.view.width
      height: InputStyle.rowHeight

      Row {
        height: parent.height
        width: contentChildren.width

        anchors.centerIn: parent
        visible: btModel.discovering

        Components.LoadingSpinner {
          id: loadingSpinner

          running: btModel.discovering
          width: parent.height
        }

        Text {
          id: discoveryInProgress

          text: footerDelegate.ListView.view.count > 2 ? qsTr("Looking for more devices") + " ..." : qsTr("Looking for devices") + " ..."
          font.pixelSize: InputStyle.fontPixelSizeNormal

          color: InputStyle.fontColor
          y: parent.height / 2 - contentHeight / 2

          wrapMode: Text.WordWrap
          elide: Text.ElideRight
        }
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

    Components.BluetoothConnectionDialog {
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
