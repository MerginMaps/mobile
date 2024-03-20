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

import "../../app/qml/components"

Column {
  id: root

  required property string name
  required property url source
  required property bool showRect
  required property bool invertColors
  property bool isCircle: false

  property color borderColor: invertColors ? __style.polarColor : __style.forestColor

  // Normal Icon
  property color iconColor: {
    if (root.isCircle) {
      if (invertColors)
        return __style.grassColor
      else
        return __style.forestColor
    } else {
      if (invertColors)
        return __style.polarColor
      else
        return __style.forestColor
    }
  }

  property color bgColor: {
    if (root.isCircle) {
      if (invertColors)
        return __style.forestColor
      else
        return __style.grassColor
    } else {
      if (invertColors)
        return __style.forestColor
      else
        return __style.polarColor
    }
  }

  Text { text: root.name; width: 250 * __dp; font.bold: true }

  Item{ width: parent.width; height: 3 }

  Row {
    anchors.horizontalCenter: parent.horizontalCenter

    Rectangle { width: 50 * __dp; height: 50 * __dp; color: root.bgColor
      MMIcon { size: __style.icon16; source: root.source; anchors.verticalCenter: parent.verticalCenter; color: root.iconColor
        Rectangle { anchors.fill: parent; color: "transparent"; border.color: root.borderColor; z: -1; visible: root.showRect }
      }
    }
    Rectangle { width: 50 * __dp; height: 50 * __dp; color: root.bgColor
      MMIcon { size: __style.icon24; source: root.source; anchors.verticalCenter: parent.verticalCenter; color: root.iconColor
        Rectangle { anchors.fill: parent; color: "transparent"; border.color: root.borderColor; z: -1; visible: root.showRect }
      }
    }
    Rectangle { width: 50 * __dp; height: 50 * __dp; color: root.bgColor
      MMIcon { size: __style.icon32; source: root.source; anchors.verticalCenter: parent.verticalCenter; color: root.iconColor
        Rectangle { anchors.fill: parent; color: "transparent"; border.color: root.borderColor; z: -1; visible: root.showRect }
      }
    }
  }
  Item{ width: parent.width; height: 3 }
  Rectangle { width: parent.width; height: 1; color: "gray" }
}
