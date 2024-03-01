/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

Item {

  width: parent.width
  height: (1 * __dp) < 1 ? 1 : 1 * __dp

  Rectangle {
    width: parent.width
    height: parent.height
    color: __style.greyColor
  }
}
