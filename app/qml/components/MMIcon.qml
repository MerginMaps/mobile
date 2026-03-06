/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls.impl

/**
 Use the size property instead of width and height
*/
Item {
  id: root

  required property url source
  property real size: __style.icon24
  property color color: __style.forestColor
  readonly property bool colorable: source.toString().endsWith("-coloroverlay.svg")

  width: size
  height: size

  // IconImage is not part of QML's public API, so this can break on Qt version change.
  // However, if we don't want to use shaders, this is the most straightforward way.
  IconImage {
    visible: !root.source.toString().endsWith("-nocoloroverlay.svg")
    source: root.source
    sourceSize.width: root.width
    sourceSize.height: root.height
    color: root.color
  }

  Image {
    visible: root.source.toString().endsWith("-nocoloroverlay.svg")
    source: root.source
    sourceSize.width: root.width
    sourceSize.height: root.height
  }
}
