/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQml.Models 2.14
import QtQuick.Layouts 1.14

import lc 1.0
import ".."

Item {
  id: root

  signal contentClicked()
  signal leftActionClicked()
  signal rightActionClicked()

  property alias content: contentContainer.children
  property alias leftAction: leftActionContainer.children
  property alias rightAction: rightActionContainer.children

  width: parent.width
  height: customStyle.fields.height

  Rectangle { // background
    width: parent.width
    height: parent.height
    border.color: customStyle.fields.normalColor
    border.width: 1 * __dp
    color: customStyle.fields.backgroundColor
    radius: customStyle.fields.cornerRadius
  }

  Item {
    id: rowlayout

    anchors {
      fill: parent
      leftMargin: customStyle.fields.sideMargin
      rightMargin: customStyle.fields.sideMargin
    }

    Item {
      id: leftActionContainer

      property bool actionAllowed: leftActionContainer.children.length > 1

      height: parent.height
      width: actionAllowed ? parent.height : 0

      MouseArea {
        width: leftActionContainer.actionAllowed ? parent.width + customStyle.fields.sideMargin : parent.width
        x: leftActionContainer.actionAllowed ? parent.x - customStyle.fields.sideMargin : parent.x
        height: parent.height
        onClicked: root.leftActionClicked()
      }
    }

    Item {
      id: contentContainer

      y: leftActionContainer.y
      x: leftActionContainer.width

      height: parent.height
      width: parent.width - ( leftActionContainer.width + rightActionContainer.width )

      MouseArea {
        width: parent.width
        height: parent.height
        onClicked: root.contentClicked()
      }
    }

    Item {
      id: rightActionContainer

      property bool actionAllowed: rightActionContainer.children.length > 1

      y: contentContainer.y
      x: contentContainer.x + contentContainer.width

      height: parent.height
      width: actionAllowed > 0 ? parent.height : 0

      MouseArea {
        width: rightActionContainer.actionAllowed ? parent.width + customStyle.fields.sideMargin : parent.width
        height: parent.height
        onClicked: root.rightActionClicked()
      }
    }
  }
}
