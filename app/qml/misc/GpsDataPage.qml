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

import "../components"
import lc 1.0
import ".."

Page {
  id: root

  property var positionKit

  signal back()

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

    background: Rectangle {
      anchors.fill: parent
      color: InputStyle.clrPanelMain
    }

    Column {

      anchors.fill: parent

      leftPadding: InputStyle.formSpacing
      topPadding: InputStyle.formSpacing

      TextRowWithTitle {
        id: coordinates

        width: parent.width

        titleText: qsTr( "Coordinates (lon, lat)" )
        text: {
          if ( !root.positionKit.position || !root.positionKit.position.x ) {
            return qsTr( "Loading data from GPS" ) // if you do not have position yet
          }
          __inputUtils.degreesString( root.positionKit.position )
        }
      }

      TextRowWithTitle {
        id: projectCoordinates

        width: parent.width

        titleText: qsTr( "Project coordinates (x, y)" )
        text: {
          if ( !root.positionKit.projectedPosition || !root.positionKit.projectedPosition.x ) {
            return qsTr( "Loading data from GPS" ) // if you do not have projected position yet
          }
          __inputUtils.formatNumber( root.positionKit.projectedPosition.x, 3 )  + ", " + __inputUtils.formatNumber( root.positionKit.projectedPosition.y, 3 )
        }
      }

      TextRowWithTitle {
        id: altitude

        width: parent.width

        titleText: qsTr( "Altitude" )
        text: {
          if ( !root.positionKit.position || !root.positionKit.position.z ) {
            return qsTr( "Loading data from GPS" ) // if you do not have position yet
          }
          __inputUtils.formatNumber( root.positionKit.position.z, 2 ) + " m"
        }
      }

      TextRowWithTitle {
        id: accuracy

        width: parent.width

        titleText: qsTr( "Accuracy (horizontal, vertical)" )
        text: {
          if ( !root.positionKit.position || !root.positionKit.position.x ) {
            return qsTr( "Loading data from GPS" ) // if you do not have position yet
          }
          __inputUtils.formatNumber( root.positionKit.accuracy, 3 ) + " m, " + __inputUtils.formatNumber( root.positionKit.verticalAccuracy, 3 ) + " m"
        }
      }

      TextRowWithTitle {
        id: sattelites

        width: parent.width

        titleText: qsTr( "Satellites (in use/view)" )
        text: root.positionKit.usedSatellitesCount + "/" + root.positionKit.satellitesInViewCount
      }

      TextRowWithTitle {
        id: speed

        width: parent.width

        titleText: qsTr( "Speed" )
        text: root.positionKit.speed < 0 ? qsTr( "Loading data from GPS" ) : __inputUtils.formatNumber( root.positionKit.speed, 2 ) + " km/h"
      }

      TextRowWithTitle {
        id: lastFix

        width: parent.width

        titleText: qsTr( "Last fix" )
        text: root.positionKit.lastGPSRead ? root.positionKit.lastGPSRead.toLocaleTimeString( Qt.locale() ) : qsTr( "Date not available" )
      }
    }
  }
}
