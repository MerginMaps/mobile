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
import QtQuick.Controls
import QtQuick.Dialogs

import mm 1.0 as MM
import MMInput

import "../components" as MMComponents
import "../dialogs" as MMDialogs
import "../inputs" as MMInputs

MMComponents.MMPage {
  id: root

  pageHeader.title: listview.showTopTitle ? internal.pageTitle : ""

  pageBottomMarginPolicy: MMComponents.MMPage.PaintBehindSystemBar

  pageContent: Item {
    width: parent.width
    height: parent.height

    MMComponents.MMListView {
      id: listview

      property bool showTopTitle: visibleArea.yPosition * height > ( headerItem.contentHeight / 2 )

      visible: __haveBluetooth

      width: parent.width
      height: parent.height

      clip: true

      model: MM.PositionProvidersModel {
        id: providersModel

        appSettings: AppSettings
      }

      header: MMComponents.MMText {
        id: headerText

        width: ListView.view.width

        text: internal.pageTitle

        font: __style.h3
        color: __style.forestColor

        wrapMode: Text.Wrap
        maximumLineCount: 2

        bottomPadding: __style.margin40
      }

      delegate: MMComponents.MMListDelegate {
        id: listdelegate

        property bool isActive: AppSettings.activePositionProviderId === model.ProviderId

        leftContent: MMComponents.MMRadioButton {
          checked: listdelegate.isActive

          // We need to duplicate mouse area here in order to not toggle the radio button immediately
          MouseArea {
            anchors.fill: parent
            onClicked: function( mouse ) {
              mouse.accepted = true
              root.constructProvider( model.ProviderType, model.ProviderId, model.ProviderName )
            }
          }
        }

        text: model.ProviderName ? model.ProviderName : qsTr( "Unknown device" )
        secondaryText: listdelegate.isActive ? PositionKit.positionProvider.stateMessage : model.ProviderDescription

        rightContent: MMComponents.MMRoundButton {
          visible: model.ProviderType !== "internal"
          iconSource: __style.deleteIcon
          onClicked: removeDialog.openDialog( model.ProviderId )
        }

        hasLine: {
          if ( index === ListView.view.count - 1 ) return false
          if ( ListView.section === "internal" ) {
            let ix = providersModel.index( index + 1, 0 )
            let type = providersModel.data( ix, MM.PositionProvidersModel.ProviderType )
            if ( type.includes("external") ) return false
          }

          return true
        }

        onClicked: root.constructProvider( model.ProviderType, model.ProviderId, model.ProviderName )
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

      onClicked: providerTypeDrawer.open()
    }

    MMComponents.MMListDrawer {
      id: providerTypeDrawer

      drawerHeader.title: qsTr("Pick the correct provider type")
      drawerHeader.titleFont: __style.t2

      list.model: ListModel {
        ListElement {
          name: qsTr( "Bluetooth provider" )
          type: "bluetooth"
        }
        ListElement {
          name: qsTr( "Network provider" )
          type: "network"
        }
      }

      list.delegate: MMComponents.MMListDelegate {
        required property string name
        required property string type

        text: name
        onClicked: {
          providerTypeDrawer.close()
          if (type === "bluetooth") {
            bluetoothDiscoveryLoader.active = true
          }
          else if (type === "network"){
            networkProviderInfoDrawer.open()
          }
        }
      }
    }

    MMComponents.MMDrawerDialog {
      id: networkProviderInfoDrawer

      title: qsTr("Network provider setup")
      imageSource: __style.externalGpsGreenImage
      description: qsTr( "To connect to the external device please specify the IP address and port below." )
      primaryButton.text: qsTr( "Confirm" )
      primaryButton.type: MMComponents.MMButton.Primary

      additionalContent: Column {
        width: parent.width
        spacing: __style.spacing20

        MMInputs.MMTextInput {
          id: aliasInput

          width: parent.width
          textFieldBackground.color: __style.lightGreenColor

          title: qsTr("Device alias")
          placeholderText: qsTr("Green device")
        }

        MMInputs.MMTextInput {
          id: ipAddressInput

          width: parent.width
          textFieldBackground.color: __style.lightGreenColor

          title: qsTr("IP address")
          placeholderText: qsTr("localhost")
        }

        MMInputs.MMTextInput {
          id: portInput

          width: parent.width
          textFieldBackground.color: __style.lightGreenColor

          title: qsTr("Port")
          placeholderText: qsTr("1234")
        }
      }

      onPrimaryButtonClicked: function() {
        close()

        const deviceAddress = ipAddressInput.text + ":" + portInput.text
        __positionKit.positionProvider = __positionKit.constructProvider( "external_ip", deviceAddress, aliasInput.text )

        providersModel.addProvider( aliasInput.text, deviceAddress, "external_ip" )
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
        if (removeDialog.providerId === "") {
          close()
          return
        }

        if ( AppSettings.activePositionProviderId === removeDialog.providerId )
        {
          // we are removing an active provider, replace it with internal provider
          root.constructProvider( "internal", "devicegps", qsTr( "Internal" ) )
        }

        providersModel.removeProvider( removeDialog.providerId )

        removeDialog.providerId = ""
      }
    }

    Loader {
      id: bluetoothDiscoveryLoader

      active: false
      sourceComponent: bluetoothDiscoveryDrawerComponent
    }

    Loader {
      id: connectingDialogLoader

      active: false
      asynchronous: true
      sourceComponent: connectionToSavedProviderDialogComponent

      function open() {
        active = true
        focus = true
      }
    }
  }

  Component {
    id: bluetoothDiscoveryDrawerComponent

    MMAddPositionProviderDrawer {
      onInitiatedConnectionTo: function ( deviceAddress, deviceName ) {
        PositionKit.positionProvider = PositionKit.constructProvider( "external_bt", deviceAddress, deviceName )

        providersModel.addProvider( deviceName, deviceAddress, "external_bt" )
        list.model.discovering = false
        close()

        connectingDialogLoader.open()
      }
      onClosed: bluetoothDiscoveryLoader.active = false

      Component.onCompleted: open()
    }
  }

  Component {
    id: connectionToSavedProviderDialogComponent

    MMBluetoothConnectionDrawer {
      onClosed: connectingDialogLoader.active = false

      // revert position provider back to internal provider
      onFailure: PositionKit.positionProvider = PositionKit.constructProvider( "internal", "devicegps", "" )

      Component.onCompleted: open()
    }
  }

  QtObject {
    id: internal

    property string pageTitle: qsTr( "Manage GPS receivers" )
  }

  function constructProvider( type, id, name ) {
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

    PositionKit.positionProvider = PositionKit.constructProvider( type, id, name )

    if ( type === "external_bt" ) {
      connectingDialogLoader.open()
    }
  }
}
