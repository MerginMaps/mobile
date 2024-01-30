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

import "../../app/qml/components"
import "../../app/qml/inputs"
import "../../app/qml/form"

Page {
  id: root

  background: Rectangle {
    color: __style.lightGreenColor
  }

  header: MMHeader {
    title: "Edit feature"

    rightMarginShift: saveButton.width

    onBackClicked: console.log("back")

    MMRoundButton {
      id: saveButton

      anchors.right: parent.right
      anchors.verticalCenter: parent.verticalCenter
      anchors.rightMargin: __style.pageMargins

      iconSource: __style.checkmarkIcon
      iconColor: __style.forestColor

      bgndColor: __style.grassColor

      onClicked: console.log("save")
    }
  }

  ColumnLayout {
    anchors.fill: parent

    MMFormTabBar {
      id: tabBar

      Layout.alignment: Qt.AlignHCenter
      Layout.maximumWidth: __style.maxPageWidth

      tabButtonsModel: ListModel {
        id: tabModel

        ListElement { Name: "Address of the object" }
        ListElement { Name: "Description of the object xyz" }
        ListElement { Name: "Meta" }
        ListElement { Name: "For future use" }
        ListElement { Name: "For the 2nd stage of survey" }
      }

      onCurrentIndexChanged: formSwipe.setCurrentIndex(tabBar.currentIndex)
    }

    SwipeView {
      id: formSwipe

      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignHCenter
      Layout.maximumWidth: __style.maxPageWidth

      clip: true

      onCurrentIndexChanged: tabBar.setCurrentIndex(formSwipe.currentIndex)

      Repeater {
        model: tabModel

        Item {

          ListView {

            anchors {
              fill: parent
              leftMargin: __style.pageMargins
              rightMargin: __style.pageMargins
            }

            model: ListModel {
              ListElement { Group: "" }
              ListElement { Group: "Group 1" }
              ListElement { Group: "Group 1" }
              ListElement { Group: "Group 2" }
              ListElement { Group: "Group 3" }
              ListElement { Group: "Group 3" }
              ListElement { Group: "Group 3" }
              ListElement { Group: "Group 3" }
              ListElement { Group: "Group 3" }
              ListElement { Group: "Group 3" }
              ListElement { Group: "Group 3" }
            }

            clip: true
            spacing: internal.formSpacing

            header: Rectangle {
              opacity: 1 // invisible
              height: 20 * __dp
            }

            section {
              property: "Group"
              delegate: sectionDelegate
              labelPositioning: ViewSection.CurrentLabelAtStart | ViewSection.InlineLabels
            }

            delegate: fieldDelegate

            footer: Rectangle {
              opacity: 1 // invisible
              height: 20 * __dp
            }
          }
        }
      }
    }
  }

  footer: MMToolbar {
    model: ObjectModel {
      MMToolbarButton { text: "Delete feature"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "Edit geometry"; iconSource: __style.editIcon; onClicked: console.log("tapped "+text) }
    }
  }

  Component {
    id: sectionDelegate

    Item {

      property string sectionTitle: section

      height: section ? 76 * __dp : 0
      width: ListView.view.width

      // section bgnd
      Rectangle {
        anchors.fill: parent;
        color: __style.lightGreenColor;
      }

      Text {
        id: sectionTitle

        text: section
        font: __style.h3
        color: __style.forestColor

        topPadding: internal.formSpacing
        bottomPadding: internal.formSpacing
      }
    }
  }

  Component {
    id: fieldDelegate

    MMInputEditor {
      width: ListView.view.width
      title: qsTr("Field title")
      placeholderText: qsTr("placeholder...")
    }
  }

  QtObject {
    id: internal

    property real formSpacing: 20 * __dp
  }
}
