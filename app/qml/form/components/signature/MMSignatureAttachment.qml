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
 * Sign now button of the signature field, shown when there is no signature yet.
 */

Rectangle {
  id: root

  color: __style.polarColor
  radius: __style.radius20

  signal signNowClicked()

  Rectangle {
    anchors.fill: parent
    anchors.margins: __style.margin20

    radius: root.radius
    color: __style.lightGreenColor

    Column {
      width: parent.width
      anchors.centerIn: parent
      spacing: 8 * __dp

      MMComponents.MMIcon {
        anchors.horizontalCenter: parent.horizontalCenter
        source: __style.redrawGeometryIcon
        size: __style.icon32
        color: root.enabled ? __style.forestColor : __style.mediumGreyColor
      }

      MMComponents.MMText {
        width: parent.width

        font: __style.p5
        text: qsTr("Sign now")
        color: __style.nightColor

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideMiddle

        visible: root.enabled
      }
    }

    MMComponents.MMSingleClickMouseArea {
      anchors.fill: parent
      onSingleClicked: root.signNowClicked()
    }
  }
}
