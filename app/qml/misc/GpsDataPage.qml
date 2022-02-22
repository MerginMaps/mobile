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
import QtQuick.Layouts 1.14

import "../components"
import ".."
import lc 1.0

Item {
  id: root

  property var mapSettings
  property string coordinatesInDegrees: __inputUtils.degreesString( __positionKit.positionCoordinate )

  property real cellWidth: root.width * 0.4

  signal back()

  Keys.onReleased: {
    if ( event.key === Qt.Key_Back || event.key === Qt.Key_Escape ) {
      event.accepted = true
      root.back()
    }
  }

  StackView {
    id: additionalContent

    anchors.fill: parent
    initialItem: gpsPageComponent
  }

  Component {
    id: gpsPageComponent

    Page {
      id: gpsPage

      focus: true
      Keys.onReleased: {
        if ( event.key === Qt.Key_Back || event.key === Qt.Key_Escape ) {
          event.accepted = true
          root.back()
        }
      }

      Component.onCompleted: forceActiveFocus()

      MapPosition {
        id: mapPositioning

        positionKit: __positionKit
        mapSettings: root.mapSettings
      }

      header: PanelHeader {

        titleText: qsTr( "GPS info" )

        height: InputStyle.rowHeightHeader
        rowHeight: InputStyle.rowHeightHeader

        color: InputStyle.clrPanelMain

        onBack: root.back()
        withBackButton: true
      }

      ScrollView {
        id: scrollPage

        anchors.fill: parent
        contentWidth: availableWidth // to only scroll vertically

        spacing: InputStyle.panelSpacing
        clip: true

        background: Rectangle {
          anchors.fill: parent
          color: InputStyle.clrPanelMain
        }

        GridLayout {
          id: gridItem

          columns: 2

          anchors {
            fill: parent
            leftMargin: 2 * InputStyle.formSpacing
            rightMargin: 2 * InputStyle.formSpacing
            topMargin: InputStyle.formSpacings
          }

          TextRowWithTitle {
            id: gpsReceiver

            Layout.fillWidth: true

            titleText: qsTr( "Source" )
            text: __positionKit.positionProvider ? __positionKit.positionProvider.name() : qsTr( "No receiver" )
          }

          Item {
            id: gpsReceiverStatusContainer

            Layout.fillWidth: true
            Layout.preferredHeight: InputStyle.rowHeight


            TextRowWithTitle {
              id: gpsReceiverStatus

              width: parent.width
              height: parent.height

              titleText: qsTr( "Status" )
              text: __positionKit.positionProvider ? __positionKit.positionProvider.stateMessage : ""
              visible: __positionKit.positionProvider && __positionKit.positionProvider.type() === "external"
            }
          }

          TextRowWithTitle {
            id: longitude

            Layout.fillWidth: true

            titleText: qsTr( "Longitude" )
            text: {
              if ( !__positionKit.hasPosition || Number.isNaN( __positionKit.longitude ) ) {
                return qsTr( "N/A" )
              }

              let coordParts = root.coordinatesInDegrees.split(", ")
              if ( coordParts.length > 1 )
                return coordParts[1]

              return qsTr( "N/A" )
            }
          }

          TextRowWithTitle {
            id: latitude

            Layout.fillWidth: true

            titleText: qsTr( "Latitude" )
            text: {
              if ( !__positionKit.hasPosition || Number.isNaN( __positionKit.latitude ) ) {
                return qsTr( "N/A" )
              }

              let coordParts = root.coordinatesInDegrees.split(", ")
              if ( coordParts.length > 1 )
                return coordParts[0]

              return qsTr( "N/A" )
            }
          }

          TextRowWithTitle {
            id: projectX

            Layout.fillWidth: true

            titleText: qsTr( "X" )
            text: {
              if ( !__positionKit.hasPosition || Number.isNaN( mapPositioning.mapPosition.x ) ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( mapPositioning.mapPosition.x, 2 )
            }
          }

          TextRowWithTitle {
            id: projectY

            Layout.fillWidth: true

            titleText: qsTr( "Y" )
            text: {
              if ( !__positionKit.hasPosition || Number.isNaN( mapPositioning.mapPosition.y ) ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( mapPositioning.mapPosition.y, 2 )
            }
          }

          TextRowWithTitle {
            id: horizontalAccuracy

            Layout.fillWidth: true

            titleText: qsTr( "Horizontal accuracy" )
            text: {
              if ( !__positionKit.hasPosition || __positionKit.horizontalAccuracy < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( __positionKit.horizontalAccuracy, 2 ) + " m"
            }
          }

          TextRowWithTitle {
            id: verticalAccuracy

            Layout.fillWidth: true

            titleText: qsTr( "Vertical accuracy" )
            text: {
              if ( !__positionKit.hasPosition || __positionKit.verticalAccuracy < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( __positionKit.verticalAccuracy, 2 ) + " m"
            }
          }

          TextRowWithTitle {
            id: altitude

            Layout.fillWidth: true

            titleText: qsTr( "Altitude" )
            text: {
              if ( !__positionKit.hasPosition || Number.isNaN( __positionKit.altitude ) ) {
                return qsTr( "N/A" )
              }
              __inputUtils.formatNumber( __positionKit.altitude, 2 ) + " m"
            }
          }

          TextRowWithTitle {
            id: fix

            Layout.fillWidth: true

            visible: __positionKit.positionProvider && __positionKit.positionProvider.type() === "external"

            titleText: qsTr( "Fix quality" )
            text: {
              if ( !__positionKit.hasPosition ) {
                return qsTr( "N/A" )
              }

              __positionKit.fix
            }
          }

          TextRowWithTitle {
            id: sattelites

            Layout.fillWidth: true

            titleText: qsTr( "Satellites (in use/view)" )
            text: {
              if ( __positionKit.satellitesUsed < 0 || __positionKit.satellitesVisible < 0 )
              {
                return qsTr( "N/A" )
              }

              __positionKit.satellitesUsed + "/" + __positionKit.satellitesVisible
            }
          }

          TextRowWithTitle {
            id: hdop

            Layout.fillWidth: true

            visible: __positionKit.positionProvider && __positionKit.positionProvider.type() === "external"

            titleText: qsTr( "HDOP" )
            text: {
              if ( !__positionKit.hasPosition || __positionKit.hdop < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( __positionKit.hdop, 2 )
            }
          }

          TextRowWithTitle {
            id: speed

            Layout.fillWidth: true

            titleText: qsTr( "Speed" )
            text: {
              if ( !__positionKit.hasPosition || __positionKit.speed < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( __positionKit.speed, 2 ) + " km/h"
            }
          }

          TextRowWithTitle {
            id: lastFix

            Layout.fillWidth: true

            titleText: qsTr( "Last fix" )
            text: __positionKit.lastRead.toLocaleTimeString( Qt.locale() ) || qsTr( "N/A" )
          }
        }
      }

      footer: Item {
        width: parent.width
        height: InputStyle.rowHeightHeader

        Rectangle {
          anchors.fill: parent
          color: InputStyle.clrPanelBackground
        }

        Row {
          anchors.fill: parent

          Item {
            width: parent.width / parent.children.length
            height: parent.height

            MainPanelButton {
              id: managePositionProviders
              width: parent.height * 0.8
              text: qsTr( "Manage GPS receivers" )
              imageSource: InputStyle.settingsIcon

              onActivated: {
                additionalContent.push( positionProviderComponent )
              }
            }
          }
        }
      }
    }
  }

  Component {
    id: positionProviderComponent
    PositionProviderPage {
      onClose: additionalContent.pop(null)
      stackView: additionalContent
      Component.onCompleted: forceActiveFocus()
    }
  }
}
