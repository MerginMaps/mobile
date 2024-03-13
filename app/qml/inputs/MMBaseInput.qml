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
import QtQml.Models
import QtQuick.Layouts

import "../components"
import "."

//! This is a base class for all inputs/form editors, do not use this in the app directly

Item {
  id: root

  signal contentClicked()
  signal leftActionClicked()
  signal rightActionClicked()

  property alias title: titleItem.text
  property alias leftAction: leftActionContainer.children
  property alias content: contentContainer.children
  property alias rightAction: rightActionContainer.children
  property string warningMsg
  property string errorMsg
  property bool hasFocus: false
  property color bgColor: __style.polarColor
  property bool hasCheckbox: false
  property alias checkboxChecked: checkbox.checked

  property real contentItemHeight: 50 * __dp

  property real spacing: 15 * __dp
  property real radius: __style.radius12

  width: parent.width
  height: mainColumn.height

  Column {
    id: mainColumn

    spacing: 6 * __dp
    anchors.left: parent.left
    anchors.right: parent.right

    Row {
      id: titleRow

      spacing: 4 * __dp
      width: parent.width
      visible: titleItem.text.length > 0

      MMCheckBox {
        id: checkbox

        small: true
        visible: root.hasCheckbox
      }
      Text {
        id: titleItem

        width: parent.width - checkbox.width - titleRow.spacing

        font: __style.p6
        wrapMode: Text.WordWrap
      }
    }

    Item {
      height: root.contentItemHeight
      anchors.left: parent.left
      anchors.right: parent.right

      Rectangle {
        id: background

        width: parent.width
        height: parent.height

        border.width: 2 * __dp
        color: root.bgColor
        radius: root.radius
        border.color: {
          if (root.hasFocus) {
            if (errorMsg.length > 0) {
              return __style.negativeColor
            }
            else if (warningMsg.length > 0) {
              return __style.warningColor
            }
            return __style.forestColor
          }
          return __style.transparentColor
        }
      }

      Row {
        height: parent.height
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: root.spacing
        anchors.rightMargin: root.spacing

        Item {
          id: leftActionContainer

          property bool actionAllowed: leftActionContainer.children.length > 1

          height: parent.height
          width: actionAllowed ? height/2 : 0

          Item {
            width: leftActionContainer.actionAllowed ? parent.width + root.spacing/2 : 0
            height: parent.height
            anchors.centerIn: parent

            MouseArea {
              anchors.fill: parent
              onReleased: root.leftActionClicked()
            }
          }
        }

        Item {
          id: contentContainer

          height: parent.height
          width: parent.width - (leftActionContainer.actionAllowed ? leftActionContainer.width : 0) - (rightActionContainer.actionAllowed ? rightActionContainer.width : 0)

          MouseArea {
            anchors.fill: parent

            onClicked: {
              root.contentClicked()
            }
          }
        }

        Item {
          id: rightActionContainer

          property bool actionAllowed: rightActionContainer.children.length > 1

          height: parent.height
          width: actionAllowed ? height/2 : 0

          Item {
            width: rightActionContainer.actionAllowed ? parent.width + root.spacing/2 : 0
            height: parent.height
            anchors.centerIn: parent

            MouseArea {
              anchors.fill: parent
              onReleased: root.rightActionClicked()
            }
          }
        }
      }
    }

    Item {
      id: messageItem

      width: parent.width
      height: msgRow.height

      Row {
        id: msgRow

        spacing: 4 * __dp

        MMIcon {
          id: msgIcon

          source: visible ? __style.errorCircleIcon : ""
          color: errorMsg.length > 0 ? __style.negativeColor : __style.warningColor
          size: __style.icon16
          visible: errorMsg.length > 0 || warningMsg.length > 0
        }
        Text {
          width: messageItem.width - msgRow.spacing - msgIcon.width

          text: root.errorMsg.length > 0 ? root.errorMsg : root.warningMsg
          font: __style.t4
          wrapMode: Text.WordWrap
          visible: root.errorMsg.length > 0 || root.warningMsg.length > 0
        }
      }
    }
  }
}
