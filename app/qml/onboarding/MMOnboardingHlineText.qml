
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
import QtQuick.Layouts

RowLayout {

  required property var title

  id: root
  spacing: 10

  Rectangle {
    width: 50
    height: 1
  }

  Label {
    text: title
  }

  Rectangle {
    width: 50
    height: 1
  }
}
