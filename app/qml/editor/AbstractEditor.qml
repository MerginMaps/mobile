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
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "../"

Item {
  id: root
  property alias content: contentContainer.children
  property alias leftAction: leftActionContainer.children
  property alias rightAction: rightActionContainer.children

  height: customStyle.fields.height
  width: parent.width

  signal contentClicked
  signal leftActionClicked
  signal rightActionClicked

  Rectangle {
    border.color: customStyle.fields.normalColor
    border.width: 1 * QgsQuick.Utils.dp
    color: customStyle.fields.backgroundColor
    height: parent.height
    radius: customStyle.fields.cornerRadius // background
    width: parent.width
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
        height: parent.height
        width: leftActionContainer.actionAllowed ? parent.width + customStyle.fields.sideMargin : parent.width
        x: leftActionContainer.actionAllowed ? parent.x - customStyle.fields.sideMargin : parent.x

        onClicked: root.leftActionClicked()
      }
    }
    Item {
      id: contentContainer
      height: parent.height
      width: parent.width - (leftActionContainer.width + rightActionContainer.width)
      x: leftActionContainer.width
      y: leftActionContainer.y

      MouseArea {
        height: parent.height
        width: parent.width

        onClicked: root.contentClicked()
      }
    }
    Item {
      id: rightActionContainer
      property bool actionAllowed: rightActionContainer.children.length > 1

      height: parent.height
      width: actionAllowed > 0 ? parent.height : 0
      x: contentContainer.x + contentContainer.width
      y: contentContainer.y

      MouseArea {
        height: parent.height
        width: rightActionContainer.actionAllowed ? parent.width + customStyle.fields.sideMargin : parent.width

        onClicked: root.rightActionClicked()
      }
    }
  }
}
