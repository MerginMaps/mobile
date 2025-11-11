/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts

import mm 1.0 as MM
import MMInput

import "../components" as MMComponents
import "./components" as MMGpsComponents

MMComponents.MMDrawer {
  id: root

  property var mapSettings

  property bool showReceiversButton: true

  signal manageReceiversClicked()

  drawerHeader.title: qsTr( "GPS info" )

  drawerContent: MMComponents.MMScrollView {

    width: parent.width
    height: root.maxHeightHit ? root.drawerContentAvailableHeight : contentHeight

    Column {
      width: parent.width

      spacing: __style.spacing40

      Item {
        width: parent.width
        height: datagrid.implicitHeight

        Rectangle {
          // horizontal line after each row in the grid
          anchors {
            fill: datagrid
            bottomMargin: datagrid.children[0].height
          }
          color: __style.greyColor
        }

        Grid {
          id: datagrid

          width: parent.width
          columns:  2

          rowSpacing: __style.margin1

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Source" )
            value: __positionKit.positionProvider ? __positionKit.positionProvider.name() : qsTr( "No receiver" )

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            id: statusCell

            width: parent.width / 2

            visible: __positionKit.positionProvider && __positionKit.positionProvider.type() === "external"

            title: qsTr( "Status" )
            value: __positionKit.positionProvider ? __positionKit.positionProvider.stateMessage : ""

            alignmentRight: Positioner.index % 2 === 1
          }

          Rectangle { // placeholder cell to keep long/lat in one line
            width: parent.width
            height: statusCell.height

            color: __style.polarColor
            visible: !statusCell.visible
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Longitude")
            value: {
              if ( !__positionKit.hasPosition || Number.isNaN( __positionKit.longitude ) ) {
                return qsTr( "N/A" )
              }

              let coordParts = internal.coordinatesInDegrees.split(", ")
              if ( coordParts.length > 1 )
                return coordParts[1]

              return qsTr( "N/A" )
            }

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Latitude" )
            value: {
              if ( !__positionKit.hasPosition || Number.isNaN( __positionKit.latitude ) ) {
                return qsTr( "N/A" )
              }

              let coordParts = internal.coordinatesInDegrees.split(", ")
              if ( coordParts.length > 1 )
                return coordParts[0]

              return qsTr( "N/A" )
            }

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "X" )
            value: {
              if ( !__positionKit.hasPosition || Number.isNaN( mapPositioning.mapPosition.x ) ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( mapPositioning.mapPosition.x, 2 )
            }

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Y" )
            value: {
              if ( !__positionKit.hasPosition || Number.isNaN( mapPositioning.mapPosition.y ) ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( mapPositioning.mapPosition.y, 2 )
            }

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Horizontal accuracy" )
            value: {
              if ( !__positionKit.hasPosition || __positionKit.horizontalAccuracy < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( __positionKit.horizontalAccuracy, 2 ) + " m"
            }

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Vertical accuracy" )
            value: {
              if ( !__positionKit.hasPosition || __positionKit.verticalAccuracy < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( __positionKit.verticalAccuracy, 2 ) + " m"
            }

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Altitude" )
            value: {
              if ( !__positionKit.hasPosition || Number.isNaN( __positionKit.altitude ) ) {
                return qsTr( "N/A" )
              }
              __inputUtils.formatNumber( __positionKit.altitude, 2 ) + " m"
            }

            alignmentRight: Positioner.index % 2 === 1
            desc: qsTr("Final value calculated with EGM96_15 geoid model.")
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Fix quality" )
            value: {
              if ( !__positionKit.hasPosition ) {
                return qsTr( "N/A" )
              }

              __positionKit.fix
            }

            visible: __positionKit.positionProvider && __positionKit.positionProvider.type() === "external"

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Satellites (in use/view)" )
            value: {
              if ( __positionKit.satellitesUsed < 0 || __positionKit.satellitesVisible < 0 )
              {
                return qsTr( "N/A" )
              }

              __positionKit.satellitesUsed + "/" + __positionKit.satellitesVisible
            }

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "HDOP" )
            value: {
              if ( !__positionKit.hasPosition || __positionKit.hdop < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( __positionKit.hdop, 2 )
            }

            visible: __positionKit.positionProvider && __positionKit.positionProvider.type() === "external"

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "VDOP" )
            value: {
              if ( !__positionKit.hasPosition || __positionKit.vdop < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( __positionKit.vdop, 2 )
            }

            visible: __positionKit.positionProvider && __positionKit.positionProvider.type() === "external"

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "PDOP" )
            value: {
              if ( !__positionKit.hasPosition || __positionKit.pdop < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( __positionKit.pdop, 2 )
            }

            visible: __positionKit.positionProvider && __positionKit.positionProvider.type() === "external"

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Speed" )
            value: {
              if ( !__positionKit.hasPosition || __positionKit.speed < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( __positionKit.speed, 2 ) + " km/h"
            }

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Last Fix" )
            value: __positionKit.lastRead.toLocaleTimeString( Qt.locale() ) || qsTr( "N/A" )

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "GPS antenna height" )
            value: AppSettings.gpsAntennaHeight > 0 ? __inputUtils.formatNumber(AppSettings.gpsAntennaHeight, 3) + " m" : qsTr( "Not set" )

            alignmentRight: Positioner.index % 2 === 1
          }
        }
      }

      MMComponents.MMButton {
        width: parent.width

        visible: root.showReceiversButton

        text: qsTr( "Manage GPS receivers" )
        onClicked: root.manageReceiversClicked()
      }
    }
  }

  MM.MapPosition {
    id: mapPositioning

    positionKit: __positionKit
    mapSettings: root.mapSettings
  }

  QtObject {
    id: internal

    property string coordinatesInDegrees: __inputUtils.degreesString( __positionKit.positionCoordinate )
  }
}
