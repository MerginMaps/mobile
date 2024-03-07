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

Drawer {
  id: root

  property bool hasHandle: false

  property alias drawerHeader: mmDrawerHeader
  property alias drawerContent: contentGroup.children

  property double drawerSpacing: __style.spacing20
  property double drawerBottomMargin: __style.margin8 // set to 0 to draw behind navigation bar

  property real maxHeight: ( ApplicationWindow.window?.height ?? 0 ) - __style.safeAreaTop

  signal backClicked()

  implicitHeight: contentHeight > maxHeight ? maxHeight : contentHeight
  implicitWidth: ApplicationWindow.window?.width ?? 0

  edge: Qt.BottomEdge
  dragMargin: 0

  // rounded background
  background: Rectangle {
    color: __style.whiteColor
    radius: internal.radius

    Rectangle {
      width: parent.width / 10
      height: 4 * __dp

      anchors.top: parent.top
      anchors.topMargin: 8 * __dp
      anchors.horizontalCenter: parent.horizontalCenter

      visible: root.hasHandle

      radius: internal.radius

      color: __style.lightGreenColor
    }

    Rectangle {
      color: __style.whiteColor
      width: parent.width
      height: parent.height / 2
      y: parent.height / 2
    }

    Keys.onReleased: function( event ) {
      if ( event.key === Qt.Key_Back || event.key === Qt.Key_Escape ) {
        root.backClicked()
        event.accepted = true
      }
    }
  }

  contentItem: Column {
    id: mainColumn

    anchors.fill: parent
    spacing: 0

    height: mmDrawerHeader.height + contentGroup.height + root.drawerSpacing

    MMDrawerHeader {
      id: mmDrawerHeader

      width: parent.width

      onCloseClicked: {
        root.close()
      }
    }

    Item {
      width: parent.width
      height: root.drawerSpacing
    }

    Item {
      id: contentGroup

      property real minLeftPadding: __style.pageMargins + __style.safeAreaLeft
      property real minRightPadding: __style.pageMargins + __style.safeAreaRight
      property real minSidesPadding: 2 * __style.pageMargins + __style.safeAreaLeft + __style.safeAreaRight

      property real leftPadding: {
        if ( parent.width > __style.maxPageWidth + minSidesPadding ) {
          let leftSideOverflow = ( parent.width - minSidesPadding - __style.maxPageWidth ) / 2
          return leftSideOverflow + minLeftPadding
        }

        return minLeftPadding
      }

      property real rightPadding: {
        if ( parent.width > __style.maxPageWidth + minSidesPadding ) {
          let rightSideOverflow = ( parent.width - minSidesPadding - __style.maxPageWidth ) / 2
          return rightSideOverflow + minRightPadding
        }

        return minRightPadding
      }

      height: childrenRect.height
      width: parent.width - leftPadding - rightPadding

      // center the content
      x: leftPadding
    }

    // Bottom spacer
    Item {
      width: parent.width
      height: root.drawerBottomMargin > 0 ? __style.safeAreaBottom + root.drawerBottomMargin :0
    }
  }

  QtObject {
    id: internal

    property real radius: 20 * __dp
  }
}
