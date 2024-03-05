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

Page {
  id: pane

  Label {
    width: parent.width / 2

    text: "DP ratio: " + __dp.toPrecision(6) + "\n" +
          "Used font: " + font.family + "\n" +
          "Safe area - top: " + __style.safeAreaTop + "\n" +
          "Safe area - right: " + __style.safeAreaRight + "\n" +
          "Safe area - left: " + __style.safeAreaLeft + "\n" +
          "Safe area - bottom: " + __style.safeAreaBottom

    anchors.centerIn: parent
    horizontalAlignment: Label.AlignHCenter
    verticalAlignment: Label.AlignVCenter
    wrapMode: Label.Wrap
    color: "red"
    font.family: "Inter"
    font.pixelSize: 20
  }
}
