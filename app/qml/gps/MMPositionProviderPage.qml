/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import mm 1.0 as MM
import MMInput

import "../components" as MMComponents
import "../dialogs" as MMDialogs

MMComponents.MMPage {
  id: root

  pageHeader.title: internal.pageTitle

  pageBottomMarginPolicy: MMComponents.MMPage.PaintBehindSystemBar

  pageContent: Item {
    width: parent.width
    height: parent.height

    MMComponents.MMListView {
      id: listview

      width: parent.width
      height: parent.height

      clip: true

      model: MM.PositionProvidersModel {
        id: providersModel

        appSettings: AppSettings
      }

      delegate: MMComponents.MMListDelegate {
        id: listdelegate

        required property var model
        required property int index

        property bool isActive: AppSettings.activePositionProviderId === listdelegate.model.ProviderId

        leftContent: MMComponents.MMRadioButton {
          checked: listdelegate.isActive

          // We need to duplicate mouse area here in order to not toggle the radio button immediately
          MouseArea {
            anchors.fill: parent
            onClicked: function( mouse ) {
              mouse.accepted = true
              root.activateProvider( listdelegate.model.ProviderType, listdelegate.model.ProviderId, listdelegate.model.ProviderName )
            }
          }
        }

        text: {
          if ( listdelegate.model.ProviderName ) return listdelegate.model.ProviderName
          return qsTr( "Unknown device" )
        }
        secondaryText: {
          if ( listdelegate.isActive ) {
            if ( listdelegate.model.ProviderType === "external_ip" )
              return PositionKit.positionProvider.stateMessage + " - " + PositionKit.positionProvider.id()
            return PositionKit.positionProvider.stateMessage
          }
          return listdelegate.model.ProviderDescription
        }

        rightContent: MMComponents.MMRoundButton {
          visible: listdelegate.model.ProviderType !== "internal"
          iconSource: __style.deleteIcon
          onClicked: removeDialog.openDialog( listdelegate.model.ProviderId )
        }

        hasLine: {
          if ( listdelegate.index === ListView.view.count - 1 ) return false
          if ( ListView.section === "internal" ) {
            let ix = providersModel.index( listdelegate.index + 1, 0 )
            let type = providersModel.data( ix, MM.PositionProvidersModel.ProviderType )
            if ( type.includes( "external" ) ) return false
          }

          return true
        }

        onClicked: root.activateProvider( listdelegate.model.ProviderType, listdelegate.model.ProviderId, listdelegate.model.ProviderName )
      }

      section {
        property: "ProviderType"
        delegate: MMComponents.MMText {
          width: ListView.view.width

          text: section === "internal" ? qsTr( "Internal receivers" ) : qsTr( "External receivers" )

          font: __style.p6
          color: __style.nightColor

          MMComponents.MMLine { width: parent.width; y: parent.height }
        }
      }

      footer: MMComponents.MMListSpacer {
        height: __style.safeAreaBottom + __style.margin8 + connectNewReceiverButton.height
      }
    }

    MMComponents.MMButton {
      id: connectNewReceiverButton

      visible: listview.visible

      width: parent.width
      anchors {
        bottom: parent.bottom
        bottomMargin: __style.safeAreaBottom + __style.margin8
      }

      text: qsTr( "Connect new receiver" )

      onClicked: {
        if ( __haveBluetooth ) {
          providerTypeDrawer.open()
        }
        else {
          networkProviderDrawer.open()
        }
      }
    }

    MMProviderTypeDrawer {
      id: providerTypeDrawer

      onProviderSelected: function( providerType ) {
        if ( providerType === "bluetooth" ) bluetoothDiscoveryLoader.active = true
        else if ( providerType === "network" ) networkProviderDrawer.open()
      }
    }

    MMNetworkProviderDrawer {
      id: networkProviderDrawer

      onConfirmed: function( alias, deviceAddress ) {
        root.activateProvider( "external_ip", deviceAddress, alias )
      }
    }

    MMComponents.MMMessage {
      visible: !listview.visible
      width: parent.width
      anchors.centerIn: parent

      image: __style.externalGpsRedImage
      title: qsTr( "Connecting to external receivers via bluetooth is not supported" )
      description: qsTr( "This function is not available on iOS. " +
                          "Your hardware vendor may provide a custom " +
                          "app that connects to the receiver and sets position. " +
                          "The app will still think it is the internal GPS of " +
                          "your phone/tablet." )
      link: __inputHelp.howToConnectGPSLink
    }

    MMDialogs.MMProviderRemoveReceiverDialog {
      id: removeDialog

      function openDialog( positionProviderId ) {
        removeDialog.providerId = positionProviderId
        visible = true
      }

      onRemoveProvider: {
        if ( !removeDialog.providerId ) {
          close()
          return
        }

        if ( AppSettings.activePositionProviderId === removeDialog.providerId )
        {
          // we are removing an active provider, replace it with internal provider
          root.activateProvider( "internal", "devicegps", qsTr( "Internal" ) )
        }

        providersModel.removeProvider( removeDialog.providerId )

        removeDialog.providerId = ""
      }
    }

    Loader {
      id: bluetoothDiscoveryLoader

      active: false
      sourceComponent: Component { MMBluetoothProviderDrawer {} }

      onLoaded: item.open()
    }

    Connections {
      target: bluetoothDiscoveryLoader.item

      function onInitiatedConnectionTo( deviceAddress, deviceName ) {
        bluetoothDiscoveryLoader.item.list.model.discovering = false
        bluetoothDiscoveryLoader.item.close()
        root.activateProvider( "external_bt", deviceAddress, deviceName )
      }

      function onClosed() { bluetoothDiscoveryLoader.active = false }
    }

    Loader {
      id: connectingDialogLoader

      property string providerType: ""

      active: false
      asynchronous: true
      sourceComponent: Component { MMExternalProviderConnectionDrawer{} }

      onLoaded: {
        item.providerType = connectingDialogLoader.providerType
        item.open()
      }

      function open( type ) {
        providerType = type
        active = true
        focus = true
      }
    }

    Connections {
      target: connectingDialogLoader.item

      function onClosed() { connectingDialogLoader.active = false }
      function onFailure() { root.activateProvider( "internal", "devicegps", "" ) }
    }
  }

  QtObject {
    id: internal

    property string pageTitle: qsTr( "Manage GPS receivers" )
  }

  function activateProvider( type, id, name ) {
    if ( type === "external_bt" ) {
      // Is bluetooth turned on?
      if ( !__inputUtils.isBluetoothTurnedOn() ) {
        __inputUtils.turnBluetoothOn()
        return
      }
    }

    if ( AppSettings.activePositionProviderId === id ) {
      return // do not construct the same provider again
    }

    providersModel.addProvider( name, id, type )
    PositionKit.positionProvider = PositionKit.constructProvider( type, id, name )

    if ( type === "external_bt" ) {
      connectingDialogLoader.open( "bluetooth" )
    }
    else if ( type === "external_ip" ) {
      connectingDialogLoader.open( "network" )
    }
  }
}
