/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQml.Models
import "../components" as MMComponents

MMComponents.MMListDrawer {
  id: root

  property string selectedProviderType: ""

  signal bluetoothSelected()
  signal networkSelected()

  drawerHeader.title: qsTr( "Connect new receiver" )
  drawerHeader.titleFont: __style.t2

  onOpened: root.selectedProviderType = ""

  list.model: ListModel {
    id: providerTypeModel

    Component.onCompleted: {
      providerTypeModel.append( [
        { name: qsTr( "Bluetooth" ), description: qsTr( "Bad Elf, Emlid, Juniper, marXact and more" ), type: "bluetooth", icon: __style.bluetoothIcon },
        { name: qsTr( "Network (TCP, UDP)" ), description: qsTr( "Emlid RS, EOS and more" ), type: "network", icon: __style.networkIcon }
      ] )
    }
  }

  list.header: MMComponents.MMText {
      width: ListView.view.width
       
      text: __inputUtils.htmlLink(
                   qsTr( "External receivers use different connection methods depending on the manufacturer. Select a connection type below, or %1check our documentation%2 for supported devices and setup instructions." ),
                   __style.nightColor,
                   __inputHelp.howToConnectGPSLink,
                   "",
                   true,
                   false
                 )
      font: __style.p5
      color: __style.nightColor
      textFormat: Text.RichText

      wrapMode: Text.Wrap
      horizontalAlignment: Text.AlignJustify
      bottomPadding: __style.margin16

      onLinkActivated: function( link ) {
        Qt.openUrlExternally( link )
      }
    }

  list.delegate: Item {
    width: ListView.view.width
    height: checkbox.height + __style.margin12

    MMComponents.MMIconCheckBoxHorizontal{
      id: checkbox

      width: parent.width
      showBorder: true
      sourceIcon: model.icon
      text: model.name
      description: model.description
      checked: root.selectedProviderType === model.type

      onClicked: {
        if ( root.selectedProviderType === model.type ) {
          root.selectedProviderType = ""
        }
        else {
          root.selectedProviderType = model.type
        }
      }
    }
  }

  list.footer: Item {
    width: ListView.view.width
    height: continueButton.height + __style.margin20

    MMComponents.MMButton {
      id: continueButton

      width: parent.width
      anchors.top: parent.top
      anchors.topMargin: __style.margin8

      text: qsTr( "Continue" )
      enabled: root.selectedProviderType !== ""

      onClicked: {
        root.close()

        if ( root.selectedProviderType === "bluetooth" ) {
          root.bluetoothSelected()
        }
        else if ( root.selectedProviderType === "network" ) {
          root.networkSelected()
        }
      }
    }
  }
}
