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

import "../../../components" as MMComponents

/*
 * Empty-state UI for the signature field, shown when no signature has been drawn yet.
 */

Rectangle {
  id: root

  color: __style.polarColor
  radius: __style.radius20

  signal signNowClicked()

  Rectangle {
    anchors.fill: parent
    anchors.margins: __style.margin10

    radius: root.radius
    color: __style.lightGreenColor

    Column {
      anchors.centerIn: parent
      spacing: 8 * __dp

      MMComponents.MMIcon {
        anchors.horizontalCenter: parent.horizontalCenter
        source: __style.redrawGeometryIcon
        color: root.enabled ? __style.forestColor : __style.mediumGreyColor
      }

      MMComponents.MMText {
        font: __style.p5
        text: qsTr("Sign now")
        color: root.enabled ? __style.forestColor : __style.mediumGreyColor

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
      }
    }

    MMComponents.MMSingleClickMouseArea {
      anchors.fill: parent
      onSingleClicked: root.signNowClicked()
    }
  }
}
