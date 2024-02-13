/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

Rectangle {
  id: root

  enum Type {Edit, Checkbox, Close, SmallCheckmark}

  required property int type
  required property url source
  required property real size
  property color iconColor: __style.forestColor

  color: __style.grassColor
  width: size
  height: size
  radius: size / 2

  MMIcon {
    anchors.centerIn: parent
    source: root.source
    color: root.iconColor
    size: __style.icon24
    width: {
      if( root.type === MMCircleIcon.Type.Edit )
        return 7 * __dp
      if( root.type === MMCircleIcon.Type.SmallCheckmark )
        return 8 * __dp
      return __style.icon24
    }
    height: {
      if( root.type === MMCircleIcon.Type.Edit )
        return 8 * __dp
      if( root.type === MMCircleIcon.Type.SmallCheckmark )
        return 6 * __dp
      return __style.icon24
    }
  }
}
