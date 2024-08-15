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

  property bool dropShadow: false

  property alias drawerHeader: mmDrawerHeader
  property alias drawerContent: contentGroup.children

  property double drawerSpacing: __style.margin10
  property double drawerBottomMargin: __style.margin8 // set to 0 to draw behind navigation bar

  property real maxHeight: ( ApplicationWindow.window?.height ?? 0 ) - __style.safeAreaTop

  // Properties useful to determine if the content should scroll or not
  readonly property bool maxHeightHit: implicitHeight >= maxHeight
  readonly property real drawerReservedVerticalSpace: topSpacer.height + mmDrawerHeader.height + contentSpacer.height + bottomSpacer.height
  readonly property real drawerContentAvailableHeight: maxHeight - drawerReservedVerticalSpace // max height for your custom content item

  property string leftButtonIcon: ""
  property string leftButtonText: ""
  property string leftButtonType: MMButton.Types.Primary

  implicitHeight: contentHeight > maxHeight ? maxHeight : contentHeight
  implicitWidth: ApplicationWindow.window?.width ?? 0

  edge: Qt.BottomEdge
  dragMargin: 0

  // rounded background
  background: Rectangle {
    color: __style.polarColor
    radius: __style.radius20

    layer.enabled: root.dropShadow ? true : false
    layer.effect: MMShadow {}

    Rectangle {
      color: __style.polarColor
      width: parent.width
      height: parent.height / 2
      y: parent.height / 2
    }
  }

  contentItem: Column {
    id: mainColumn

    anchors.fill: parent
    spacing: 0
    focus : true

    Keys.onReleased: function( event ) {
      if ( event.key === Qt.Key_Back || event.key === Qt.Key_Escape ) {
        root.close()
        event.accepted = true
      }
    }

    MMListSpacer {
      id: topSpacer
      height: __style.margin10
    }

    MMDrawerHeader {
      id: mmDrawerHeader

      width: parent.width

      leftButtonIcon: root.leftButtonIcon
      leftButtonText: root.leftButtonText
      leftButtonType: root.leftButtonType

      onCloseClicked: {
        root.close()
      }
    }

    Item {
      id: contentSpacer

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
      id: bottomSpacer

      width: parent.width
      height: root.drawerBottomMargin > 0 ? __style.safeAreaBottom + root.drawerBottomMargin :0
    }
  }
}
