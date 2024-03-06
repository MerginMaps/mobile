/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../components"

Rectangle {
  id: root

  property alias workspaceName: wsTitle.text
  property alias workspaceDetails: wsDesc.text

  property bool isHighlighted: false

  signal clicked()

  implicitHeight: childrenRect.height

  radius: __style.radius12
  color: root.isHighlighted ? __style.forestColor : __style.whiteColor

  Column {
    width: parent.width
    height: childrenRect.height

    Text {
      id: wsTitle

      width: parent.width

      elide: Text.ElideMiddle

      topPadding: __style.margin20
      leftPadding: __style.margin20
      rightPadding: __style.margin20
      bottomPadding: wsDesc.text ? 0 : __style.margin20

      font: __style.t3
      color: root.isHighlighted ? __style.whiteColor : __style.nightColor
    }

    MMListSpacer { height: __style.margin12; visible: wsDesc.text }

    Text {
      id: wsDesc

      width: parent.width

      leftPadding: __style.margin20
      rightPadding: __style.margin20
      bottomPadding: __style.margin20

      wrapMode: Text.Wrap
      maximumLineCount: 2
      elide: Text.ElideRight

      lineHeightMode: Text.FixedHeight
      lineHeight: 24 * __dp

      font: __style.p6
      color: root.isHighlighted ? __style.whiteColor : __style.nightColor

      visible: root.workspaceDetails
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: function( mouse ) {
      mouse.accepted = true
      root.clicked()
    }
  }

}
