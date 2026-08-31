/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
pragma ComponentBehavior: Bound

import QtQuick
import QtQml.Models

import "../components" as MMComponents

MMComponents.MMListDrawer {
  id: root

  signal providerSelected( string providerType )

  drawerHeader.title: qsTr( "Connect new receiver" )
  drawerHeader.titleFont: __style.t2

  onOpened: list.currentIndex = -1

  Component.onCompleted: () => {
    root.list.currentIndex = -1
  }

  list.model: ListModel {
    id: providerTypeModel

    Component.onCompleted: () => {
      if ( PositionKit.hasBluetoothSupport ) {
        providerTypeModel.append( { name: qsTr( "Bluetooth" ), description: qsTr( "Bad Elf, Emlid, Juniper, marXact and more" ), type: "bluetooth" } )
      }

      providerTypeModel.append( { name: qsTr( "Network (TCP, UDP)" ), description: qsTr( "Emlid RS, EOS and more" ), type: "network" } )

      if ( PositionKit.hasTrimbleSupport ) {
        providerTypeModel.append( { name: "Trimble", description: qsTr( "Trimble receivers via Trimble Mobile Manager" ), type: "trimble" } )
      }
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
    required property string name
    required property string description
    required property string type
    required property int index

    width: ListView.view.width
    height: checkbox.height + __style.margin12

    MMComponents.MMCheckBoxHorizontal {
      id: checkbox

      width: parent.width
      showBorder: true
      text: parent.name
      description: parent.description
      checked: parent.ListView.isCurrentItem

      onClicked: () => {
        if ( parent.ListView.isCurrentItem ) {
          parent.ListView.view.currentIndex = -1
        }
        else {
          parent.ListView.view.currentIndex = parent.index
        }
      }
    }
  }

  list.footer: MMComponents.MMButton {
    width: parent.width

    text: qsTr( "Continue" )
    enabled: root.list.currentIndex !== -1

    onClicked: () => {
      const providerType = providerTypeModel.get( root.list.currentIndex ).type
      root.close()

      root.providerSelected( providerType )
    }
  }
}
