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

Row {
  id: root

  property string headerTitle
  property int step: -1 // -1 no step bar shown; 1, 2, 3
  property bool backVisible: true
  property color backColor: __style.whiteColor
  property font titleFont: __style.t4

  signal backClicked

  width: parent.width
  spacing: 5 * __dp

  Row {
    id: backButton

    width: 60 * __dp

    MMBackButton {
      visible: root.backVisible
      color: root.backColor

      onClicked: root.backClicked()
    }

    Item {
      id: space

      width: 20 * __dp
      height: 1
      visible: backButton.visible
    }
  }

  Text {
    anchors.verticalCenter: parent.verticalCenter
    width: {
      if(backButton.visible || progressBar.visible)
        return root.width - (backButton.visible ? backButton.width + root.spacing : 0)
            - progressBar.width - root.spacing
      return root.width
    }
    text: root.headerTitle
    font: root.titleFont
    color: __style.forestColor
    wrapMode: Text.WordWrap
    horizontalAlignment: Text.AlignHCenter
  }

  Item {
    width: progressBar.width
    height: progressBar.height
    anchors.verticalCenter: parent.verticalCenter

    MMProgressBar {
      id: progressBar

      width: 60 * __dp
      height: 4 * __dp

      color: __style.grassColor
      progressColor: __style.forestColor
      visible: root.step > 0
      position: root.step > 0 ? root.step / 3 : 0
    }
  }
}
