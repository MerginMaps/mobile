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

  property real offset: __style.toolbarHeight + __style.safeAreaBottom + 40

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  Label {
    id: safeAreaLabel
    y: 20
    text: "safe bottom area: " + __style.safeAreaBottom
  }

  CheckBox {
    y: 35
    checked: true
    text: "use safe bottom area"

    onCheckedChanged: {
      if (checked)
        __style.safeAreaBottom = 20
      else
        __style.safeAreaBottom = 0
    }
  }

  Label {
    y: pane.offset  - 20
    text: "MMSelectableToolbar"
  }

  MMSelectableToolbar {
    id: selectableToolbar

    y: pane.offset

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

  Label {
    y: 2 * pane.offset  - 20
    text: "MMToolbar"
  }

  MMToolbar {
    y: 2 * pane.offset

    model: ObjectModel {
      MMToolbarLongButton { text: "Long button"; iconSource: __style.editCircleIcon; iconColor: __style.forestColor; onClicked: console.log("tapped "+text) }
    }
  }

  Label {
    y: 3 * pane.offset  - 20
    text: "MMToolbar"
  }

  MMToolbar {

    y: 3 * pane.offset

    model: ObjectModel {
      MMToolbarButton { text: "Sync"; iconSource: __style.syncIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "Layers"; iconSource: __style.layersIcon; onClicked: console.log("tapped "+text) }
    }
  }

  Label {
    y: 4 * pane.offset  - 20
    text: "MMToolbar"
  }

  MMToolbar {

    y: 4 * pane.offset

    model: ObjectModel {
      MMToolbarButton { text: "1/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "2/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "3/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "4/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "5/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "6/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "7/8"; menuButtonRightText: "active"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "8/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
    }
  }

  Label {
    y: 5 * pane.offset  - 20
    text: "MMToolbar"
  }

  MMToolbar {

    y: 5 * pane.offset

    model: ObjectModel {
      MMToolbarButton { text: "Delete"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "Edit Geometry"; iconSource: __style.editIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "Save"; iconSource: __style.doneCircleIcon; onClicked: console.log("tapped "+text) }
    }
  }
}
