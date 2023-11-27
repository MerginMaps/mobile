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

import ".."

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

  readonly property real spacing: 15 * __dp

  width: parent.width
  height: mainColumn.height

  Column {
    id: mainColumn

    spacing: 6 * __dp
    anchors.left: parent.left
    anchors.right: parent.right
    //      anchors.leftMargin: root.spacing
    //      anchors.rightMargin: root.spacing

    Text {
      id: titleItem

      width: parent.width
      font: StyleV2.p6
      wrapMode: Text.WordWrap
      visible: text.length > 0
    }

    Item {
      height: 50 * __dp
      anchors.left: parent.left
      anchors.right: parent.right

      Rectangle {
        id: background

        width: parent.width
        height: parent.height
        border.color: root.hasFocus ? ( errorMsg.length > 0 ? StyleV2.negativeColor : warningMsg.length > 0 ? StyleV2.warningColor : StyleV2.forestColor ) : StyleV2.transparentColor
        border.width: 2 * __dp
        color: StyleV2.whiteColor
        radius: 12 * __dp
      }

      Row {
        height: parent.height
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: root.spacing
        anchors.rightMargin: root.spacing

        Item {
          id: leftActionContainer

          property bool actionAllowed: leftActionContainer.children.length > 1 && (leftActionContainer.children[1]?.visible ?? false)

          height: parent.height
          width: actionAllowed ? height/2 : 0

          Rectangle {
            color: StyleV2.transparentColor
            anchors.centerIn: parent
            width: leftActionContainer.actionAllowed ? parent.width + root.spacing/2 : 0
            height: parent.height

            MouseArea {
              anchors.fill: parent
              onReleased: root.leftActionClicked()
            }
          }
        }

        Item {
          id: contentContainer

          height: parent.height
          width: parent.width - (leftActionContainer.actionAllowed ? leftActionContainer.width : 0) - (rightActionContainer.actionAllowed ? rightActionContainer.width : 0) //- 2 * root.spacing - ( leftActionContainer.actionAllowed ? root.spacing : 0 )

          MouseArea {
            anchors.fill: parent

            onClicked: {
              root.contentClicked()
            }
          }
        }

        Item {
          id: rightActionContainer

          property bool actionAllowed: rightActionContainer.children.length > 1 && (rightActionContainer.children[1]?.visible ?? false)

          height: parent.height
          width: actionAllowed ? height/2 : 0

          Rectangle {
            color: StyleV2.transparentColor
            anchors.centerIn: parent
            width: rightActionContainer.actionAllowed ? parent.width + root.spacing/2 : 0
            height: parent.height

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

          source: visible ? StyleV2.errorIcon : ""
          color: errorMsg.length > 0 ? StyleV2.negativeColor : StyleV2.warningColor
          visible: errorMsg.length > 0 || warningMsg.length > 0
        }
        Text {
          text: root.errorMsg.length > 0 ? root.errorMsg : root.warningMsg
          font: StyleV2.t4
          wrapMode: Text.WordWrap
          width: messageItem.width - msgRow.spacing - msgIcon.width
          visible: root.errorMsg.length > 0 || root.warningMsg.length > 0
        }
      }
    }
  }
}
