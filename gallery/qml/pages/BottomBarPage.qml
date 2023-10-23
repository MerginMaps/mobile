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

import "../../app/qmlV2/component"

Page {
  id: pane

  MMTextArea {
    width: parent.width
    maxHeight: 300
    autoHeight: true
    text: "Note about button priority numbers\n The button will be shown:\n \
    0 - always in Bottom bar (BB)\n \
    1 - when BB will contain enough space for 1 button\n \
    2 - when BB will contain enough space for 2 buttons\n \
    ...\n \
    -1 - when BB will not contain space for all buttons\n\n \
All invisible button will be shown\n \
in menu of Advenced '-1' button"
  }

  MMBottomBar {
    state: MMBottomBar.States.First
    Text {
      color: "white"; leftPadding: 20; anchors.bottom: parent.bottom
      text: "Delete:2 - EditGeometry:2 - Advanced:-1"
    }
  }
  MMBottomBar {
    state: MMBottomBar.States.Second; anchors.bottomMargin: 100
    Text {
      color: "white"; leftPadding: 20; anchors.bottom: parent.bottom
      text: "Delete:0 - EditGeometry:0"
    }
  }
  MMBottomBar { state: MMBottomBar.States.Third; anchors.bottomMargin: 200
    Text {
      color: "white"; leftPadding: 20; anchors.bottom: parent.bottom
      text: "Delete:2 - EditGeometry:3 - Advanced:-1 - Save:0"
    }

    onDeleteClicked: console.log("Delete clicked")
    onEditGeometryClicked: console.log("Edit geometry clicked")
    onSaveClicked: console.log("Save clicked")
  }
}
