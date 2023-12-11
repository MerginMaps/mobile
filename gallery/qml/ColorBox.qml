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
import QtQuick.Controls.Basic

Column {
  spacing: 5
  id: root

  property var color
  property var text

  Label {
      text: root.text
  }

  Rectangle {
      width: 40
      height: 40
      color: root.color
      border.color: "gray"
  }

}
