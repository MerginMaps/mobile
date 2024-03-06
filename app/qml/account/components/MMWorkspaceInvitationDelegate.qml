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

import "../../components"

Rectangle {
  id: root

  property alias text: textDelegate.text

  signal accepted()
  signal rejected()

  implicitHeight: 80 * __dp

  radius: 12 * __dp
  color: __style.whiteColor

  RowLayout {

    spacing: __style.spacing12

    anchors {
      fill: parent
      margins: __style.margin20
    }

    Text {
      id: textDelegate

      Layout.fillWidth: true
      Layout.fillHeight: true

      font: __style.t3
      elide: Text.ElideMiddle

      verticalAlignment: Text.AlignVCenter
    }

    MMRoundButton {
      Layout.fillHeight: true
      Layout.preferredWidth: height

      iconSource: __style.closeIcon

      iconColor: __style.grapeColor
      bgndColor: __style.negativeColor

      onClicked: root.rejected()
    }

    MMRoundButton {

      Layout.fillHeight: true
      Layout.preferredWidth: height

      iconSource: __style.checkmarkIcon

      iconColor: __style.forestColor
      bgndColor: __style.positiveColor

      onClicked: root.accepted()
    }
  }
}
