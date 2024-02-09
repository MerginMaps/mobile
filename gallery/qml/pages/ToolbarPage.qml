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

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  MMSelectableToolbar {
    id: selectableToolbar

    anchors {
      left: parent.left
      right: parent.right
      bottom: parent.bottom
    }

    Component.onCompleted: index = 1

    model: ObjectModel {
      MMSelectableToolbarButton {
        width: Math.floor((pane.width - 40 * __dp) / 3)
        text: "Home"
        iconSource: __style.homeIcon
        selectedIconSource: __style.homeFilledIcon
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
        selectedIconSource: __style.projectsFilledIcon
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
        selectedIconSource: __style.globalFilledIcon
        checked: selectableToolbar.index === 2
        onClicked: {
          selectableToolbar.index = 2
          console.log("tapped "+text)
        }
      }
    }
  }

  MMToolbar {

    anchors {
      left: parent.left
      right: parent.right
      bottom: parent.bottom
      bottomMargin: 360
    }

    model: ObjectModel {
      MMToolbarLongButton { text: "Long button"; iconSource: __style.doneIcon; onClicked: console.log("tapped "+text) }
    }
  }

  MMToolbar {

    anchors {
      left: parent.left
      right: parent.right
      bottom: parent.bottom
      bottomMargin: 240
    }

    model: ObjectModel {
      MMToolbarButton { text: "Sync"; iconSource: __style.sync2Icon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "Layers"; iconSource: __style.layersIcon; onClicked: console.log("tapped "+text) }
    }
  }

  MMToolbar {

    anchors {
      left: parent.left
      right: parent.right
      bottom: parent.bottom
      bottomMargin: 120
    }

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
  }

  MMToolbar {

    anchors {
      left: parent.left
      right: parent.right
      bottom: parent.bottom
      bottomMargin: 480
    }

    model: ObjectModel {
      MMToolbarButton { text: "Delete"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "Edit Geometry"; iconSource: __style.editIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "Save"; iconSource: __style.done2Icon; onClicked: console.log("tapped "+text) }
    }
  }
}
