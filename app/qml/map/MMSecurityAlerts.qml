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

import "../components"

/*
 * Banner at the top of the map shown while the GPS position is inside a security zone
 * configured in the Mergin Maps QGIS plugin (see SecurityAlertsController).
 * Danger zones are red, warning zones amber. Closing the banner hides it until the user
 * enters another zone.
 */

Item {
  id: root

  //! Distance from the top of the map, e.g. below the routing card
  property real topOffset: 0

  readonly property var alerts: controller.activeAlerts
  readonly property bool shown: alerts.length > 0 && !internal.dismissed

  MM.SecurityAlertsController {
    id: controller

    qgsProject: __activeProject.qgsProject
    positionKit: PositionKit

    onActiveAlertsChanged: internal.dismissed = false

    onZoneEntered: function( message, level ) {
      __inputUtils.log( "Security alerts", "Entered zone: " + message )
    }
  }

  Connections {
    target: __activeProject

    function onProjectReloaded( qgsProject ) {
      controller.reload()
    }
  }

  Rectangle {
    id: banner

    visible: root.shown

    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
      topMargin: Math.max( root.topOffset, __style.safeAreaTop ) + __style.margin10
      leftMargin: __style.margin10
      rightMargin: __style.margin10
    }

    readonly property bool danger: root.alerts.length > 0 && root.alerts[0].level === "danger"

    height: bannerLayout.implicitHeight + 2 * __style.margin12
    radius: __style.radius12
    color: danger ? "#C62828" : "#F4CB46"
    border.color: danger ? "#7F0000" : "#B38F00"
    border.width: 2 * __dp

    layer.enabled: true
    layer.effect: MMShadow {}

    // keep touches on the banner away from the map below
    MouseArea {
      anchors.fill: parent
    }

    RowLayout {
      id: bannerLayout

      anchors {
        fill: parent
        margins: __style.margin12
      }

      spacing: __style.margin12

      Text {
        text: "⚠"
        font.pixelSize: 30 * __dp
        color: banner.danger ? "white" : "#3A2A00"
      }

      ColumnLayout {
        Layout.fillWidth: true
        spacing: __style.margin4

        MMText {
          Layout.fillWidth: true
          text: root.alerts.length > 0 ? root.alerts[0].message : ""
          font: __style.t2
          color: banner.danger ? "white" : "#3A2A00"
          wrapMode: Text.WordWrap
          maximumLineCount: 4
        }

        MMText {
          Layout.fillWidth: true
          visible: root.alerts.length > 1
          text: qsTr( "and %n more zone(s)", "", root.alerts.length - 1 )
          font: __style.p6
          color: banner.danger ? "white" : "#3A2A00"
        }
      }

      MMRoundButton {
        iconSource: __style.closeIcon
        bgndColor: "transparent"
        iconColor: banner.danger ? "white" : "#3A2A00"
        onClicked: internal.dismissed = true
      }
    }
  }

  QtObject {
    id: internal

    property bool dismissed: false
  }
}
