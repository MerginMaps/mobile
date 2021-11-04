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
  property var compass

  signal back()

  PositionDirection {
    id: positionDirection

    compass: root.compass
    positionKit: root.positionKit
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
        text: root.positionKit.position.x + ", " + root.positionKit.position.y
      }

      TextRowWithTitle {
        id: projectCoordinates

        width: parent.width

        titleText: qsTr( "Project coordinates (x, y)" )
        text: root.positionKit.projectedPosition.x + ", " + root.positionKit.projectedPosition.y
      }

      TextRowWithTitle {
        id: altitude

        width: parent.width

        titleText: qsTr( "Altitude" )
        text: root.positionKit.position.z
      }

      TextRowWithTitle {
        id: accuracy

        width: parent.width

        titleText: qsTr( "Accuracy (horizontal, vertical)" )
        text: root.positionKit.accuracy + ", " + root.positionKit.verticalAccuracy
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
        text: root.positionKit.speed < 0 ? qsTr( "No data from GPS" ) : root.positionKit.speed + " km/h"
      }

      TextRowWithTitle {
        id: bearing

        width: parent.width

        titleText: qsTr( "Bearing (from GPS)" )
        text: root.positionKit.direction < 0 ? qsTr( "No data from GPS" ) : root.positionKit.direction + " deg"
      }

      TextRowWithTitle {
        id: bearingPositionDirection

        width: parent.width

        titleText: qsTr( "Bearing (from position direction)" )
        text: positionDirection.direction + " deg"
      }

      TextRowWithTitle {
        id: bearingCompass

        width: parent.width

        titleText: qsTr( "Bearing (from internal compass)" )
        text: root.compass.reading ? root.compass.reading.azimuth + " deg" : qsTr( "Compass unavailable" )
      }

      TextRowWithTitle {
        id: compassCalibration

        width: parent.width

        titleText: qsTr( "Compass calibration level" )
        text: root.compass.reading ? root.compass.reading.calibrationLevel * 100 + "%" : qsTr( "Compass unavailable" )
      }

      TextRowWithTitle {
        id: lastFix

        width: parent.width

        titleText: qsTr( "Last fix" )
        text: root.positionKit.lastGPSRead ? root.positionKit.lastGPSRead.toLocaleString( Qt.locale() ) : qsTr( "Date not available" )
      }
    }
  }
}
