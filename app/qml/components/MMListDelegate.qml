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

  signal clicked()

  property string text
  property string secondaryText

  readonly property int scrollSpace: (ListView.view && ListView.view.scrollSpace) ? ListView.view.scrollSpace : 0
  property alias leftContent: leftContentGroup.children
  property alias rightContent: rightContentGroup.children

  property bool hasLine: {
    // calculate automatically when this item is a delegate in list
    if ( typeof index != "undefined" ) {
      if ( ListView?.view ?? false ) {
        return index < ListView.view.count - 1
      }
    }

    return true
  }

  property real verticalSpacing: root.secondaryText ? __style.margin8 : __style.margin20

  implicitWidth: ListView?.view?.width ?? 0  // in case ListView is injected as attached property (usually it is)
  implicitHeight: contentLayout.implicitHeight
  height: visible ? implicitHeight : 0.1 // hide invisible items, for some reason setting 0 does not work ¯\_(ツ)_/¯
  width: implicitWidth - ListView.view.scrollSpace

  MouseArea {
    anchors.fill: contentLayout
    onClicked: function( mouse ) {
      mouse.accepted = true
      root.clicked()
    }
  }

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
          textFormat: Text.PlainText

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
