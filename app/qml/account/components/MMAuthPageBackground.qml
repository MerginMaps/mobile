/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

// Use as background for MMPage to achieve "drawer-like" visual in auth pages
Rectangle {
    color: __style.forestColor

    Rectangle {
      anchors {
        fill: parent
        topMargin: Math.max( __style.safeAreaTop, __style.margin54 )
      }

      color: __style.whiteColor
      radius: __style.radius20

      Rectangle {
        // in order to hide the bottom radius of parent

        anchors {
          left: parent.left
          right: parent.right
          bottom: parent.bottom
        }

        height: 2 * __style.margin54

        color: __style.whiteColor
      }
    }
  }
