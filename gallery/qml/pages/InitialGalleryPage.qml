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

  //FontLoader { id: fontx; source: "qrc:/fonts/SquarePeg-Regular.ttf" }
  Label {
    width: parent.width / 2
    text: "DP ratio: ~" + Math.round(__dp * 1000) / 1000 + "\n" +
          "Used font: " + font.family
    anchors.centerIn: parent
    horizontalAlignment: Label.AlignHCenter
    verticalAlignment: Label.AlignVCenter
    wrapMode: Label.Wrap
    color: "red"
    font.family: "Inter"
    font.pixelSize: 20
    //font.family: fontx.font.family
  }
}
