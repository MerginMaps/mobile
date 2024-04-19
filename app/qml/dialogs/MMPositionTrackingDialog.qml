/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components" as MMComponents

MMComponents.MMDrawerDialog {
  id: root

  property bool trackingActive: false
  property string distanceTraveled
  property string trackingStartedAt

  signal trackingBtnClicked()

  title: qsTr("Position tracking")
  imageSource: root.trackingActive ? __style.positionTrackingRunningImage : __style.positionTrackingStartImage
  description: root.trackingActive ? qsTr( "Mergin Maps can track your position on this project." ) : qsTr( "Track your routes even with your screen off. Your records are stored in a separate layer. Finalised tracks are synced like any other feature." )
  primaryButton.text: root.trackingActive ? qsTr( "Stop tracking" ) : qsTr( "Start tracking" )
  primaryButton.type: root.trackingActive ? MMComponents.MMButton.Secondary : MMComponents.MMButton.Primary

  additionalContent: Item {
    width: parent.width
    height: childrenRect.height

    Row {
      width: parent.width
      spacing: __style.margin10

      Column {
        width: parent.width / 2 - parent.spacing / 2

        MMComponents.MMText {
          width: parent.width

          text: qsTr("Tracked distance")
          font: __style.p6
          color: __style.nightColor
        }

        MMComponents.MMText {
          width: parent.width

          text: root.trackingActive ? root.distanceTraveled : "-"
          font: __style.t3
          color: __style.nightColor
        }
      }

      Column {
        width: parent.width / 2 - parent.spacing / 2

        MMComponents.MMText {
          width: parent.width

          text: qsTr("Tracking time")
          horizontalAlignment: Text.AlignRight
          font: __style.p6
          color: __style.nightColor
        }

        MMComponents.MMText {
          width: parent.width

          text: root.trackingActive ? root.trackingStartedAt : "-"
          horizontalAlignment: Text.AlignRight
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
