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
import "../../app/qmlV2/"

Page {
  id: pane

  MMToolbar {
    model: ObjectModel {
      MMToolbarLongButton { text: "Long button"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
    }
    anchors.bottomMargin: 360 * __dp
  }

  MMToolbar {
    model: ObjectModel {
      MMToolbarButton { text: "aaa"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "bbb"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
    }
    anchors.bottomMargin: 240 * __dp
  }

  MMToolbar {
    model: ObjectModel {
      MMToolbarButton { text: "1/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "2/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "3/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "4/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "5/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "6/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "7/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "8/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
    }
    anchors.bottomMargin: 120 * __dp
  }

  MMToolbar {
    model: ObjectModel {
      MMToolbarButton { text: qsTr("Delete"); iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: qsTr("Edit Geometry"); iconSource: __style.editIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: qsTr("Save"); iconSource: __style.doneIcon; type: MMToolbarButton.Button.Save; onClicked: console.log("tapped "+text) }
    }
  }
}
