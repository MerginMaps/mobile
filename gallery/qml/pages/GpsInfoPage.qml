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
import "../../app/qml"

Page {
  id: pane

  Column {
    width: parent.width
    spacing: 10

    MMButton {
      text: "Gps Data Page"
      onClicked: drawer1.visible = true
    }
  }

  MMGpsDataDrawer {
    id: drawer1
    title: "Gps Info"
    visible: false
  }
}
