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

  property real offset: __style.toolbarHeight + __style.safeAreaBottom + 70

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  Label {
    id: safeAreaLabel
    x: __style.safeAreaLeft
    y: 20
    text: "safe bottom area: " + __style.safeAreaBottom
  }

  CheckBox {
    y: 35
    x: __style.safeAreaLeft
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
    x: __style.safeAreaLeft
    y: 60
    text: "MMToolbar"
    font.pixelSize: 40
  }

  MMToolbar {
    id: selectableToolbar

    y: pane.offset

    Component.onCompleted: index = 2

    model: ObjectModel {
      MMToolbarButton {
        text: "Home"
        iconSource: __style.homeIcon
        iconSourceSelected: __style.homeFilledIcon
        onClicked: console.log("tapped "+text)
      }
      MMToolbarButton {
        text: "Projects"
        iconSource: __style.projectsIcon
        iconSourceSelected: __style.projectsFilledIcon
        onClicked: console.log("tapped "+text)
      }
      MMToolbarButton {
        text: "Explore"
        iconSource: __style.globalIcon
        iconSourceSelected: __style.globalFilledIcon
        onClicked: console.log("tapped "+text)
      }
    }
  }

  MMToolbar {
    y: 2 * pane.offset

    model: ObjectModel {
      MMToolbarButton { text: "Long button"; iconSource: __style.editCircleIcon; iconColor: __style.forestColor; onClicked: console.log("tapped "+text) }
    }
  }

  MMToolbar {

    y: 3 * pane.offset

    model: ObjectModel {
      MMToolbarButton {
        text: "Sync"
        iconSource: __style.syncIcon
        iconRotateAnimationRunning: true
        onClicked: {
          iconRotateAnimationRunning = !iconRotateAnimationRunning
          console.log("tapped "+text)
        }
      }
      MMToolbarButton { text: "Layers"; iconSource: __style.layersIcon; onClicked: console.log("tapped "+text) }
    }
  }

  MMToolbar {

    y: 4 * pane.offset

    model: ObjectModel {
      MMToolbarButton { text: "Delete"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "Edit Geometry"; iconSource: __style.editIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "Save"; iconSource: __style.doneCircleIcon; onClicked: console.log("tapped "+text) }
    }
  }

  MMToolbar {
    id: mainToolbar

    anchors.bottom: parent.bottom

    model: ObjectModel {
      MMToolbarButton { text: "1/8"; iconSource: __style.addIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "2/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text); enabled: false }
      MMToolbarButton { text: "3/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "4/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "5/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text); visible: false }
      MMToolbarButton { text: "6/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text); enabled: false }
      MMToolbarButton { text: "7/8"; active: true; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "8/8"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
    }
  }
}
