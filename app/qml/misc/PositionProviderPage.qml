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

  property var positionKit

  signal close

  header: Components.PanelHeader {
    id: header

    height: InputStyle.rowHeightHeader
    width: parent.width
    color: InputStyle.clrPanelBackground
    rowHeight: InputStyle.rowHeightHeader
    titleText: "Position Providers"

    onBack: root.close()
    withBackButton: true
  }

  ListView {
    id: view

    anchors.fill: parent

    model: PositionProvidersModel {
      id: btModel

      appSettings: __appSettings
    }

    delegate: Rectangle {
      id: providerDelegate

      width: ListView.view.width
      height: InputStyle.rowHeight

      border.color: "black"
      border.width: 2 * __dp

      Row {
        id: row

        anchors.fill: parent

        RadioButton {
          id: isActiveButton

          width: parent.height
          height: parent.height

          checked: providerDelegate.ListView.isCurrentItem

//          indicator: InputStyle.activeButtonColor

        }

        Column {
          width: row.width - isActiveButton.width - removeIcon.width
          height: row.height

          Text {
            id: deviceName

            width: parent.width
            height: parent.height * 0.5

            text: model.ProviderName
          }

          Text {
            id: deviceAddress
            width: parent.width
            height: parent.height * 0.5

            text: model.ProviderDescription + " (" + model.ProviderId + ")"
          }
        }

        Image {
          id: removeIcon

          width: parent.height / 2
          sourceSize.width: parent.height / 2

          source: InputStyle.removeIcon
          visible: model.CanBeDeleted

          ColorOverlay {
            anchors.fill: removeIcon
            source: removeIcon
            color: InputStyle.activeButtonColor
          }
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          if ( model.ProviderId === "internal" )
          {
            root.positionKit.positionProvider = root.positionKit.constructProvider( "internal" )
          }
          else if ( model.ProviderId === "simulated" )
          {
            root.positionKit.positionProvider = root.positionKit.constructProvider( "simulated" )
          }
          else
          {
            root.positionKit.positionProvider = root.positionKit.constructProvider( "external", model.ProviderId )
          }
        }
      }
    }

    footer: Rectangle {
      height: InputStyle.rowHeightHeader
      implicitWidth: ListView.view.width * 0.8

      Components.TextWithIcon {
        width: parent.width
        height: parent.height
        source: InputStyle.plusIcon
        text: qsTr( "Add new provider" )
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          bluetoothDiscoveryLoader.active = true
          bluetoothDiscoveryLoader.focus = true
        }
      }
    }
  }

  Loader {
    id: bluetoothDiscoveryLoader

    asynchronous: true
    active: false
    sourceComponent: bluetoothDiscoveryComponent
  }

  Component {
    id: bluetoothDiscoveryComponent

    AddPositionProviderPage {
      positionKit: root.positionKit

      height: root.height + header.height
      width: root.width

      onClose: bluetoothDiscoveryLoader.active = false
    }
  }
}
