/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../components" as MMComponents

/*
 * Recording indicator for the feature form header.
 * Shows that an audio field records while the user is on another tab or scrolled away.
 * Tapping it goes to the field, the stop button stops the recording.
 */

Rectangle {
  id: root

  property real duration: 0 // milliseconds

  signal clicked()
  signal stopClicked()

  // draws attention to the indicator, e.g. when another field asks to record
  function nudge() {
    nudgeAnimation.restart()
  }

  implicitWidth: contentRow.implicitWidth + __style.margin16 + __style.margin6
  implicitHeight: __style.row40

  radius: height / 2
  color: __style.negativeColor

  MouseArea {
    anchors.fill: parent
    onClicked: root.clicked()
  }

  Row {
    id: contentRow

    anchors {
      left: parent.left
      leftMargin: __style.margin16
      verticalCenter: parent.verticalCenter
    }

    spacing: __style.margin8

    Rectangle {
      anchors.verticalCenter: parent.verticalCenter

      width: __style.margin10
      height: width
      radius: width / 2

      color: __style.grapeColor

      SequentialAnimation on opacity {
        running: root.visible
        loops: Animation.Infinite

        NumberAnimation { to: 0.3; duration: 600 }
        NumberAnimation { to: 1; duration: 600 }
      }
    }

    MMComponents.MMText {
      anchors.verticalCenter: parent.verticalCenter

      font: __style.t4
      color: __style.grapeColor
      text: {
        const totalSeconds = Math.max( 0, Math.floor( root.duration / 1000 ) )
        return String( Math.floor( totalSeconds / 60 ) ).padStart( 2, "0" ) + ":" + String( totalSeconds % 60 ).padStart( 2, "0" )
      }
    }

    MMComponents.MMRoundButton {
      anchors.verticalCenter: parent.verticalCenter

      implicitWidth: 32 * __dp
      implicitHeight: 32 * __dp

      bgndColor: __style.grapeColor
      bgndHoverColor: __style.earthColor
      iconColor: __style.polarColor
      iconSource: __style.stopIcon

      onClicked: root.stopClicked()
    }
  }

  SequentialAnimation {
    id: nudgeAnimation

    loops: 2

    NumberAnimation { target: root; property: "scale"; to: 1.1; duration: 150; easing.type: Easing.OutQuad }
    NumberAnimation { target: root; property: "scale"; to: 1; duration: 150; easing.type: Easing.InQuad }
  }
}
