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
import QtQuick.Layouts

import "../components"

MMDrawer {
  id: root

  property bool trackingActive: false
  property string distanceTraveled
  property string trackingStartedAt

  signal trackingBtnClicked()

  picture: root.trackingActive ? __style.positionTrackingRunningImage : __style.positionTrackingStartImage
  bigTitle: qsTr("Position tracking")
  description: root.trackingActive ? qsTr("Mergin Maps can track your position on this project."): qsTr("Track your routes even with your screen off. Your records are stored in a separate layer. Finalised tracks are synced like any other feature.")
  primaryButton: root.trackingActive ? qsTr("Stop tracking") : qsTr("Start tracking")

  specialComponent: trackingInfoPanel

  Component {
    id: trackingInfoPanel
    Item {
      width: root.width - 2 * __style.pageMargins
      height: 100

      Column {
        anchors.left: parent.left
        anchors.leftMargin: __style.pageMargins
        height: parent.height
        Text {
          text: qsTr("Tracked distance")
          font: __style.p6
          color: __style.nightColor
        }
        Text {
          text: root.trackingActive ? root.distanceTraveled : "-"
          font: __style.t3
          color: __style.nightColor
        }
      }

      Column {
        anchors.right: parent.right
        anchors.rightMargin: __style.pageMargins
        height: parent.height

        Text {
          text: qsTr("Started at")
          font: __style.p6
          color: __style.nightColor
        }
        Text {
          text: root.trackingActive ? root.trackingStartedAt : "-"
          font: __style.t3
          color: __style.nightColor
        }
      }
    }
  }

  onPrimaryButtonClicked: {
    close()
    root.trackingBtnClicked()
  }
}
