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

import lc 1.0

import "../" // import InputStyle singleton
import "../components" as Components


Page {
  id: root

  property var provider

  signal close

  header: Components.PanelHeader {
    id: header

    height: InputStyle.rowHeightHeader
    width: parent.width
    color: InputStyle.clrPanelBackground
    rowHeight: InputStyle.rowHeightHeader
    titleText: "Position Providers"

    onBack: {
      btModel.discovering = false
      root.close()
    }
    withBackButton: true
  }

  // TODO: remove this (later), it is there just for test purposes. Will be fixed in future PR
  PositionKit {
    id: pos
  }

  ListView {
    id: view

    anchors.fill: parent

    model: BluetoothDiscoveryModel {
      id: btModel
      discovering: true
    }

    delegate: Rectangle {

      width: ListView.view.width
      height: InputStyle.rowHeight

      border.color: "black"
      border.width: 2 * __dp

      Column {
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.topMargin: 5

        Text {
          id: deviceName

          width: parent.width
          height: parent.height * 0.5

          text: model.DeviceName
        }

        Text {
          id: deviceAddress
          width: parent.width
          height: parent.height * 0.5

          text: model.DeviceAddress + ", signal strength: " + model.SignalStrength
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          console.log( "Selected device: ", model.DeviceAddress )
          pos.positionProvider = pos.constructProvider( "external", model.DeviceAddress )
          root.provider = provider
        }
      }
    }

    footer: Text {
      id: discoveryInProgress

      width: ListView.view.width
      height: InputStyle.rowHeight

      leftPadding: 5
      topPadding: 5

      visible: btModel.discovering
      text: ListView.view.count > 2 ? qsTr("Looking for more devices ...") : qsTr("Looking for devices ...")
    }
  }
}
