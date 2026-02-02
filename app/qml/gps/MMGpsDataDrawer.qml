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
            value: {
              if ( PositionKit.positionProvider ) {
                PositionKit.positionProviderName()
              } else {
                qsTr( "No receiver" )
              }
            }

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            id: statusCell

            width: parent.width / 2

            visible: PositionKit.positionProvider && PositionKit.positionProvider.type() === "external"

            title: qsTr( "Status" )
            value: PositionKit.positionProvider ? PositionKit.positionProvider.stateMessage : ""

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
              if ( !PositionKit.hasPosition || Number.isNaN( PositionKit.longitude ) ) {
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
              if ( !PositionKit.hasPosition || Number.isNaN( PositionKit.latitude ) ) {
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
              if ( !PositionKit.hasPosition || Number.isNaN( mapPositioning.mapPosition.x ) ) {
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
              if ( !PositionKit.hasPosition || Number.isNaN( mapPositioning.mapPosition.y ) ) {
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
              if ( !PositionKit.hasPosition || PositionKit.horizontalAccuracy < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( PositionKit.horizontalAccuracy, 2 ) + " m"
            }

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Vertical accuracy" )
            value: {
              if ( !PositionKit.hasPosition || PositionKit.verticalAccuracy < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( PositionKit.verticalAccuracy, 2 ) + " m"
            }

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Altitude" )
            value: {
              if ( !PositionKit.hasPosition || Number.isNaN( PositionKit.altitude ) ) {
                return qsTr( "N/A" )
              }
              __inputUtils.formatNumber( PositionKit.altitude, 2 ) + " m"
            }

            alignmentRight: Positioner.index % 2 === 1
            desc: PositionKit.positionCrs3DGeoidModelName().length > 0 ? qsTr(("Orthometric height, using %1 geoid").arg(PositionKit.positionCrs3DGeoidModelName())) : ""
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Fix quality" )
            value: {
              if ( !PositionKit.hasPosition ) {
                return qsTr( "N/A" )
              }

              PositionKit.fix
            }

            visible: PositionKit.positionProvider && PositionKit.positionProvider.type() === "external"

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Satellites (in use/view)" )
            value: {
              if ( PositionKit.satellitesUsed < 0 || PositionKit.satellitesVisible < 0 )
              {
                return qsTr( "N/A" )
              }

                PositionKit.satellitesUsed + "/" + PositionKit.satellitesVisible
            }

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "HDOP" )
            value: {
              if ( !PositionKit.hasPosition || PositionKit.hdop < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( PositionKit.hdop, 2 )
            }

            visible: PositionKit.positionProvider && PositionKit.positionProvider.type() === "external"

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "VDOP" )
            value: {
              if ( !PositionKit.hasPosition || PositionKit.vdop < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( PositionKit.vdop, 2 )
            }

            visible: PositionKit.positionProvider && PositionKit.positionProvider.type() === "external"

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "PDOP" )
            value: {
              if ( !PositionKit.hasPosition || PositionKit.pdop < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( PositionKit.pdop, 2 )
            }

            visible: PositionKit.positionProvider && PositionKit.positionProvider.type() === "external"

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Speed" )
            value: {
              if ( !PositionKit.hasPosition || PositionKit.speed < 0 ) {
                return qsTr( "N/A" )
              }

              __inputUtils.formatNumber( PositionKit.speed, 2 ) + " km/h"
            }

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Last Fix" )
            value: PositionKit.lastRead.toLocaleTimeString( Qt.locale() ) || qsTr( "N/A" )

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "GPS antenna height" )
            value: AppSettings.gpsAntennaHeight > 0 ? __inputUtils.formatNumber(AppSettings.gpsAntennaHeight, 3) + " m" : qsTr( "Not set" )

            alignmentRight: Positioner.index % 2 === 1
          }

          MMGpsComponents.MMGpsDataText {
            width: parent.width / 2

            title: qsTr( "Geoid separation" )
            value: {
              if ( !PositionKit.hasPosition || Number.isNaN( PositionKit.geoidSeparation ) ) {
                return qsTr( "N/A" )
              }
              __inputUtils.formatNumber( PositionKit.geoidSeparation, 2 ) + " m"
            }

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

    positionKit: PositionKit
    mapSettings: root.mapSettings
  }

  QtObject {
    id: internal

    property string coordinatesInDegrees: __inputUtils.degreesString( PositionKit.positionCoordinate )
  }
}
