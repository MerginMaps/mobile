/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import ".."

/*
 * General purpose Text component
 */

Text {
  color: InputStyle.fontColor

  elide: Text.ElideRight
  wrapMode: Text.WordWrap

  font.pixelSize: InputStyle.fontPixelSizeSmall
}
