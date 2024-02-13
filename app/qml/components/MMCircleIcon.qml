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

  enum Type {Edit, Checkmark, Delete}

  required property int type
  property real size: __style.icon40
  property color iconColor: __style.forestColor

  color: __style.grassColor
  width: size
  height: size
  radius: size / 2

  MMIcon {
    anchors.centerIn: parent
    color: root.iconColor
    source: {
      if( root.type === MMCircleIcon.Type.Edit )
        return __style.editIcon
      if( root.type === MMCircleIcon.Type.Checkmark )
        return __style.checkmarkIcon
      if( root.type === MMCircleIcon.Type.Delete )
        return __style.deleteIcon
    }
    size: {
      if( root.type === MMCircleIcon.Type.Edit )
        return __style.icon16
      return __style.icon24
    }
  }
}
