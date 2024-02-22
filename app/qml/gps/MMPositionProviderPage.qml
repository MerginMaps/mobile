/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import lc 1.0
import "../components"

Page {
  id: root

  signal close

  function constructProvider( type, id, name )
  {
    if ( type === "external" )
    {
      // Is bluetooth turned on?
      if ( !__inputUtils.isBluetoothTurnedOn() )
      {
        __inputUtils.turnBluetoothOn()
        return
      }
    }

    if ( __appSettings.activePositionProviderId === id )
    {
      return // do not construct the same provider again
    }

     __positionKit.positionProvider = __positionKit.constructProvider( type, id, name )

    if ( type === "external" )
    {
      dialogLoader.active = true
      dialogLoader.focus = true
    }
  }

  header: MMHeader {
    id: header

    width: parent.width
    color: __style.lightGreenColor
    onBackClicked: root.close()
  }

  background: Rectangle {color: __style.lightGreenColor}

  focus: true

  Keys.onReleased: function( event ) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      close()
    }
  }

  Text {
    id: title

    anchors.left: parent.left
    anchors.leftMargin: __style.pageMargins
    anchors.top: parent.top
    anchors.topMargin: __style.margin40

    text: qsTr( "Manage GPS receivers" )
    font: __style.h3
    color: __style.forestColor
  }

  MMButton {
    id: connectNewReceiverButton

    visible: false

    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.leftMargin: __style.pageMargins
    anchors.bottomMargin: __style.pageMargins

    width: parent.width - 2 * __style.pageMargins
    text: qsTr( "Connect new receiver" )

    onClicked: {
      bluetoothDiscoveryLoader.active = true
    }
  }

  ListView {
    id: view

    anchors.left: parent.left
    anchors.leftMargin: __style.pageMargins
    anchors.top: title.bottom
    anchors.topMargin: __style.margin40

    width: root.width - 2 * __style.pageMargins

    model: PositionProvidersModel {
      id: providersModel

      appSettings: __appSettings
    }

    section {
      property: "ProviderType"
      delegate: Text {
        property string sectionTitle: section === "internal" ? qsTr( "Internal receivers" ) : qsTr( "External receivers" )

        text: sectionTitle
        width: ListView.view.width
        font: __style.p6
        color: __style.nightColor
        horizontalAlignment: Text.AlignLeft
      }
    }

    delegate: Rectangle {
      id: providerDelegate

      property bool isActiveProvider: __appSettings.activePositionProviderId === model.ProviderId

      width: ListView.view.width
      height: __style.row49
      color: __style.lightGreenColor

      MouseArea {
        anchors.fill: parent
        onClicked: root.constructProvider( model.ProviderType, model.ProviderId, model.ProviderName )
      }

      Row {
        id: row

        anchors.fill: parent

        RadioButton {
          id: isActiveButton

          width: parent.height
          height: parent.height

          checkable: false
          checked: providerDelegate.isActiveProvider

          indicator: Rectangle {
            width: isActiveButton.height / 2.3
            height: isActiveButton.height / 2.3

            anchors.centerIn: isActiveButton

            color: providerDelegate.isActiveProvider ? __style.forestColor : __style.lightGreenColor
            radius: __style.margin12
            border.color: __style.forestColor

            MMIcon {
              size: __style.icon24
              source: __style.doneCircleIcon
              x: parent.width / 2 - width / 2
              y: parent.height / 2 - height / 2
              visible: providerDelegate.isActiveProvider
              color: __style.grassColor
            }
          }

          // We need to duplicate mouse area here in order to handle clicks from RadioButton
          MouseArea {
            anchors.fill: parent
            onClicked: root.constructProvider( model.ProviderType, model.ProviderId, model.ProviderName )
          }
        }

        Column {
          width: row.width - isActiveButton.width - removeIconContainer.width
          height: providerDelegate.height

          Text {
            id: deviceName

            text: model.ProviderName ? model.ProviderName : qsTr( "Unknown device" )

            verticalAlignment: Text.AlignBottom
            elide: Text.ElideRight
            color: __style.nightColor
            font: __style.t3
          }

          Text {
            id: deviceSecondaryText

            text: model.ProviderDescription

            verticalAlignment: Text.AlignTop
            elide: Text.ElideRight
            color: __style.nightColor
            font: __style.p6
          }
        }

        MMRoundButton {
          id: removeIconContainer

          anchors.verticalCenter: parent.verticalCenter

          height: parent.height
          width: height

          bgndColor: __style.whiteColor
          iconSource: __style.deleteIcon
          visible: model.ProviderType === "external"

          onClicked: removeDialog.openDialog( model.ProviderId )
        }
      }
    }

    Component.onCompleted: {
      // select appropriate footer, on iOS say that you can not connect via BT
      if ( __haveBluetooth )
      {
        connectNewReceiverButton.visible = true
      }
      else
      {
        view.footer = btNotSupportedComponent
      }
    }
  }

  Component {
    id: btNotSupportedComponent

    MMExternalGpsNotSupportedComponent {
      link: __inputHelp.howToConnectGPSLink
      width: ListView.view.width
    }
  }

  Loader {
    id: bluetoothDiscoveryLoader

    sourceComponent: bluetoothDiscoveryComponent
    active: false
    asynchronous: true
    anchors.fill: parent
    onActiveChanged: {
      if ( active )
      {
        bluetoothDiscoveryLoader.item?.open()
      }
    }
  }

  Component {
    id: bluetoothDiscoveryComponent

    MMAddPositionProviderDrawer {
      onInitiatedConnectionTo: function( deviceAddress, deviceName ) {
        providersModel.addProvider( deviceName, deviceAddress )
      }
      onClose: bluetoothDiscoveryLoader.active = false
    }
  }

  Loader {
    id: dialogLoader

    sourceComponent: connectionToSavedProviderDialogBlueprint
    active: false
    asynchronous: true
    anchors.fill: parent
    onActiveChanged: {
      if ( active )
      {
        dialogLoader.item?.open()
      }
    }
  }

  Component {
    id: connectionToSavedProviderDialogBlueprint

    MMBluetoothConnectionDrawer {
      id: connectionToSavedProviderDialog
      onClosed: dialogLoader.active = false
    }
  }

  MessageDialog {
    id: removeDialog

    function openDialog( providerId )
    {
      relatedProviderId = providerId
      visible = true
    }

    property string relatedProviderId

    title: qsTr( "Remove receiver" )
    text: qsTr( "Do you want to remove receiver from the list of recent receivers?" )
    buttons: MessageDialog.Ok | MessageDialog.Cancel

    onButtonClicked: function(clickedButton) {
      if (clickedButton === MessageDialog.Ok) {
        if (relatedProviderId === "") {
          close()
          return
        }

        if ( __appSettings.activePositionProviderId === relatedProviderId )
        {
          // we are removing an active provider, replace it with internal provider
          root.constructProvider( "internal", "devicegps", qsTr( "Internal" ) )
        }

        providersModel.removeProvider( relatedProviderId )
      }
      removeDialog.relatedProviderId = ""
      close()
    }
  }
}
