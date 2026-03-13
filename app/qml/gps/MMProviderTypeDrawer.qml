/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components" as MMComponents
import "../account/components" as MMAccountComponents

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
        { name: qsTr( "Bluetooth" ), description: qsTr( "Connect via Bluetooth" ), type: "bluetooth", icon: __style.bluetoothIcon },
        { name: qsTr( "Network" ), description: qsTr( "Connect via IP address" ), type: "network", icon: __style.networkIcon }
      ] )
    }
  }

  list.header: MMComponents.MMText {
    width: ListView.view.width

    text: qsTr( "This function is not available on iOS. Your hardware vendor may provide a custom app that connects to the receiver and sets position. Mergin Maps will still think it is the internal GPS of your phone/tablet." )

    font: __style.p5
    color: __style.nightColor

    wrapMode: Text.Wrap
    bottomPadding: __style.margin16
  }

  list.delegate: Item {
    width: ListView.view.width
    height: checkbox.height + __style.margin12

    MMAccountComponents.MMIconCheckBoxHorizontal {
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
