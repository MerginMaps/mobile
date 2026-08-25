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
import "../../components" as MMComponents

MMComponents.MMListView {
  id: root

  implicitHeight: __style.row45

  spacing: 0

  clip: true
  orientation: ListView.Horizontal

  interactive: contentWidth > width

  header: MMComponents.MMListSpacer { width: __style.margin20 }
  footer: MMComponents.MMListSpacer { width: __style.margin20 }

  delegate: Control {
    id: tabDelegate

    property bool isSelected: ListView.isCurrentItem

    height: __style.row45
    width: contentItem.implicitWidth

    focusPolicy: Qt.NoFocus

    background: Rectangle {
      visible: tabDelegate.isSelected
      color: __style.grassColor
      radius: __style.radius30
    }

    contentItem: MMComponents.MMText {
      text: model.Name

      font: __style.t4

      leftPadding: __style.margin20
      rightPadding: __style.margin20
    }

    MouseArea {
      anchors.fill: parent
      onClicked: root.currentIndex = index
    }
  }
}

