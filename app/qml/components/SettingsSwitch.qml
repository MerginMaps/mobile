/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.6
import QgsQuick 0.1 as QgsQuick
import "." // import Components
import "../" // import InputStyle

Switch {
  id: switchComp
  anchors.right: parent.right
  anchors.rightMargin: InputStyle.panelMargin
  anchors.verticalCenter: parent.verticalCenter
  bgndColorActive: InputStyle.softGreen
  bgndColorInactive: InputStyle.panelBackgroundDark
  implicitHeight: parent.height * 0.45
}
