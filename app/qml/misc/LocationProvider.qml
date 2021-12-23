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

  signal close

  header: Components.PanelHeader {
    id: header

    height: InputStyle.rowHeightHeader
    width: parent.width
    color: InputStyle.clrPanelBackground
    rowHeight: InputStyle.rowHeightHeader
    titleText: "Location Providers"

    onBack: root.close()
    withBackButton: true
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

      Text {
        id: deviceName

        width: parent.width
        height: parent.height * 0.8

        text: model.DeviceName ? model.DeviceAddress : model.DeviceName
      }

      Text {
        id: deviceAddress

        width: parent.width
        height: parent.height * 0.2

        text: model.DeviceName ? model.DeviceAddress : ""
      }

      MouseArea {
        anchors.fill: parent
        onClicked: console.log( "Selected device: ", model.DeviceAddress )
      }
    }
  }

  Text {
    id: discoveryInProgress

    anchors.centerIn: parent

    visible: btModel.discovering
    text: qsTr("Looking for devices ...")
  }
}
