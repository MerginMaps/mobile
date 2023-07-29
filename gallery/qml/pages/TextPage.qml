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

Item {
  Rectangle {
    anchors.centerIn: parent
    color: "white"
    width: 200
    height: 200
    radius: 20
    Text {
      anchors.centerIn: parent
      text: "Text test place"
    }
  }
}
