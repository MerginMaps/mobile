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

import "../../components" as MMComponents

Item {
  id: root

  property alias title: titletxt.text
  property alias value: valuetxt.text

  property string desc: ""
  property bool alignmentRight: false

  implicitHeight: contentColumn.implicitHeight

  Rectangle {
    anchors.fill: parent
    color: __style.polarColor
  }

  Column {
    id: contentColumn

    topPadding: __style.margin8
    bottomPadding: __style.margin8

    width: parent.width

    spacing: 0

    RowLayout {
      width: parent.width
      spacing: __style.margin10

      MMComponents.MMText {
        id: titletxt

        leftPadding: alignmentRight ? __style.margin4 : 0
        rightPadding: alignmentRight ? 0 : __style.margin4

        font: __style.p6

        horizontalAlignment: alignmentRight ? Text.AlignRight : Text.AlignLeft
        Layout.alignment: root.alignmentRight? Qt.AlignRight: Qt.AlignLeft
        Layout.fillWidth: root.alignmentRight
      }

      MMComponents.MMIcon {
        id: infoIcon
        source: __style.infoIcon
        visible: root.desc
        Layout.alignment: root.alignmentRight? Qt.AlignRight | Qt.AlignBaseline : Qt.AlignLeft | Qt.AlignBaseline
        Layout.preferredWidth: __style.icon16
        Layout.preferredHeight: __style.icon16

        TapHandler{
          gesturePolicy: TapHandler.ReleaseWithinBounds
          margin: __style.margin10
          onTapped: () => infoPopup.open()
        }
      }

      MMComponents.MMListSpacer{
        visible: !root.alignmentRight
        Layout.fillWidth: !root.alignmentRight
      }
    }

    MMComponents.MMText {
      id: valuetxt

      leftPadding: alignmentRight ? __style.margin4 : 0
      rightPadding: alignmentRight ? 0 : __style.margin4

      width: parent.width - leftPadding - rightPadding
      x: leftPadding

      font: __style.t3
      color: __style.nightColor

      horizontalAlignment: alignmentRight ? Text.AlignRight : Text.AlignLeft
    }
  }

  MMComponents.MMPopup {
    id: infoPopup
    y: ( -root.height / 2 ) - __style.margin8
    MMComponents.MMText {
      font: __style.p6
      text: root.desc
    }
  }
}
