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
import QtQuick.Dialogs 1.3

import lc 1.0

import "../" // import InputStyle singleton
import "../components" as Components

Page {
  id: root

  property var stackView

  signal close

  function constructProvider( type, id )
  {
    if ( __appSettings.activePositionProviderId === id )
      return // do not construct the same provider again

    if ( type === "external" )
    {
      connectionToSavedProviderDialog.open()
    }

    __positionKit.positionProvider = __positionKit.constructProvider( type, id )
  }

  header: Components.PanelHeader {
    id: header

    height: InputStyle.rowHeightHeader
    width: parent.width
    color: InputStyle.clrPanelMain
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr( "GPS receivers" )

    onBack: root.close()
    withBackButton: true
  }

  focus: true

  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      close()
    }
  }

  ListView {
    id: view

    anchors.fill: parent

    model: PositionProvidersModel {
      id: providersModel

      appSettings: __appSettings
    }

    section {
      property: "ProviderType"
      delegate: Components.RichTextBlock {
        property string sectionTitle: section === "internal" ? qsTr( "Internal receivers" ) : qsTr( "External receivers" )

        text: sectionTitle
        width: ListView.view.width
        horizontalAlignment: Text.AlignLeft
      }
    }

    delegate: Rectangle {
      id: providerDelegate

      width: ListView.view.width
      height: InputStyle.rowHeight

      MouseArea {
        anchors.fill: parent
        onClicked: root.constructProvider( model.ProviderType, model.ProviderId )
      }

      Row {
        id: row

        anchors.fill: parent
        anchors.leftMargin: InputStyle.panelMargin
        anchors.rightMargin: InputStyle.panelMargin
        anchors.bottomMargin: 5 * __dp
        anchors.topMargin: 5 * __dp

        RadioButton {
          id: isActiveButton

          width: parent.height
          height: parent.height

          checkable: false
          checked: __appSettings.activePositionProviderId === model.ProviderId

          indicator: Rectangle {
            implicitWidth: isActiveButton.height / 2.3
            implicitHeight: isActiveButton.height / 2.3

            anchors.centerIn: isActiveButton

            radius: InputStyle.circleRadius
            border.color: InputStyle.darkGreen

            Rectangle {
              width: parent.width / 1.5
              height: parent.height / 1.5


              x: parent.width / 2 - width / 2
              y: parent.height / 2 - height / 2

              radius: InputStyle.circleRadius
              color: InputStyle.darkGreen
              visible: __appSettings.activePositionProviderId === model.ProviderId
            }
          }

          // We need to duplicate mouse area here in order to handle clicks from RadioButton
          MouseArea {
            anchors.fill: parent
            onClicked: root.constructProvider( model.ProviderType, model.ProviderId )
          }
        }

        Column {
          width: row.width - isActiveButton.width - removeIconContainer.width
          height: row.height

          Text {
            id: deviceName

            width: parent.width
            height: parent.height * 0.5

            text: model.ProviderName ? model.ProviderName : qsTr( "Unknown device" )

            elide: Text.ElideRight
            color: InputStyle.fontColor
            font.pixelSize: InputStyle.fontPixelSizeNormal
          }

          Text {
            id: deviceSecondaryText

            width: parent.width
            height: parent.height * 0.5

            text: model.ProviderDescription

            elide: Text.ElideRight
            color: InputStyle.secondaryFontColor
            font.pixelSize: InputStyle.fontPixelSizeSmall
          }
        }

        Item {
          id: removeIconContainer

          height: parent.height
          width: parent.height

          enabled: model.ProviderType === "external"

          Image {
            id: removeIcon

            anchors.centerIn: parent

            width: parent.height / 2
            sourceSize.width: parent.height / 2

            source: InputStyle.removeIcon
            visible: parent.enabled
          }

          ColorOverlay {
            anchors.fill: removeIcon
            source: removeIcon
            color: InputStyle.darkGreen
            visible: parent.enabled
          }

          MouseArea {
            anchors.fill: parent
            onClicked: removeDialog.openDialog( model.ProviderId )
          }
        }
      }

      Rectangle {
        property bool separatorVisible: {
          // items that have separator: all items in external but the last one
          if ( model.ProviderType === "internal" ) return false
          else return index < providerDelegate.ListView.view.count - 1
        }

        width: providerDelegate.width / 1.5
        height: 2 * __dp
        visible: separatorVisible
        color: InputStyle.panelBackgroundLight

        anchors.top: row.bottom
        anchors.horizontalCenter: parent.horizontalCenter
      }
    }

    footer: Rectangle {
      height: InputStyle.rowHeightHeader
      width: ListView.view.width

      Components.TextWithIcon {
        width: parent.width
        height: parent.height
        source: InputStyle.plusIcon

        fontColor: InputStyle.activeButtonColorOrange
        iconColor: InputStyle.activeButtonColorOrange

        leftPadding: InputStyle.panelMargin

        text: qsTr( "Connect new receiver" )
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          let page = root.stackView.push( bluetoothDiscoveryComponent )
          page.focus = true
        }
      }
    }
  }

  Component {
    id: bluetoothDiscoveryComponent

    AddPositionProviderPage {
      height: root.height + header.height
      width: root.width

      onInitiatedConnectionTo: providersModel.addProvider( deviceName, deviceAddress )
      onClose: root.stackView.pop()
    }
  }

  Loader {
    id: dialogLoader

    sourceComponent: connectionToSavedProviderDialogBlueprint
    active: false
    asynchronous: true

    onLoaded: item.open()
  }

  Component {
    id: connectionToSavedProviderDialogBlueprint

    Components.BluetoothConnectionDialog {
      id: connectionToSavedProviderDialog

      width: root.width * 0.8
      height: root.height / 2

      anchors.centerIn: parent
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
    icon: StandardIcon.Warning
    standardButtons: StandardButton.Ok | StandardButton.Cancel

    //! Using onButtonClicked instead of onAccepted,onRejected which have been called twice
    onButtonClicked: {
      if (clickedButton === StandardButton.Ok) {
        if (relatedProviderId === "")
          return

        if ( __appSettings.activePositionProviderId == relatedProviderId )
        {
          // we are removing an active provider, replace it with internal provider
          root.constructProvider( "internal", "devicegps" )
        }

        providersModel.removeProvider( relatedProviderId )
      }
      else if (clickedButton === StandardButton.Cancel) {
        removeDialog.relatedProviderId = ""
        visible = false
      }
    }
  }
}
