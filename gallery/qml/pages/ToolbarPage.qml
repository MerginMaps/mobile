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

import "../../app/qml/components"
import "../../app/qml/"

Page {
  id: pane

  MMSelectableToolbar {
    id: selectableToolbar

    Component.onCompleted: index = 1

    model: ObjectModel {
      MMSelectableToolbarButton {
        width: Math.floor((pane.width - 40 * __dp) / 3)
        text: "Home"
        iconSource: __style.homeIcon
        checked: selectableToolbar.index === 0
        onClicked: {
          selectableToolbar.index = 0
          console.log("tapped "+text)
        }
      }
      MMSelectableToolbarButton {
        width: Math.floor((pane.width - 40 * __dp) / 3)
        text: "Projects"
        iconSource: __style.projectsIcon
        checked: selectableToolbar.index === 1
        onClicked: {
          selectableToolbar.index = 1
          console.log("tapped "+text)
        }
      }
      MMSelectableToolbarButton {
        width: Math.floor((pane.width - 40 * __dp) / 3)
        text: "Explore"
        iconSource: __style.globalIcon
        checked: selectableToolbar.index === 2
        onClicked: {
          selectableToolbar.index = 2
          console.log("tapped "+text)
        }
      }
    }
    anchors.bottomMargin: 480 * __dp
  }

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
      MMToolbarButton { text: "Delete"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "Edit Geometry"; iconSource: __style.editIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "Save"; iconSource: __style.doneIcon; type: MMToolbarButton.Button.Save; onClicked: console.log("tapped "+text) }
    }
  }
}
