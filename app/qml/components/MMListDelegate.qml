/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts

//
// MMListDelegate item should be used within various lists
// It has left and right content that can be used to add MMIcon, MMSwitch, badges and more
// See gallery for more details
//

Item {
  id: root

  property string text
  property string secondaryText

  property alias leftContent: leftContentGroup.children
  property alias rightContent: rightContentGroup.children

  property bool hasLine: true

  property real verticalSpacing: root.secondaryText ? __style.margin8 : __style.margin20

  implicitHeight: contentLayout.implicitHeight

  Column {
    id: contentLayout

    width: parent.width

    Item { width: 1; height: verticalSpacing }

    RowLayout {
      // content
      width: parent.width

      spacing: __style.margin12

      Item {
        id: leftContentGroup

        Layout.preferredHeight: childrenRect.height
        Layout.preferredWidth: childrenRect.width

        visible: children.length > 0
      }

      Column {
        Layout.fillWidth: true

        spacing: 0

        MMText {
          width: parent.width

          font: __style.t3
          text: root.text

          color: __style.nightColor
        }

        MMText {
          width: parent.width

          visible: root.secondaryText

          font: __style.p6
          text: root.secondaryText

          color: __style.nightColor
        }
      }

      Item {
        id: rightContentGroup

        Layout.preferredHeight: childrenRect.height
        Layout.preferredWidth: childrenRect.width

        visible: children.length > 0
      }
    }

    Item { width: 1; height: verticalSpacing }

    Rectangle {
      width: parent.width
      height: __style.row1

      visible: root.hasLine
      color: __style.greyColor
    }
  }
}
