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
import QtQuick.Controls.Basic
import "."
import ".."

Drawer {
  id: control

  property alias picture: picture.source
  property alias title: title.text
  property alias description: description.text
  property alias boundedDescription: boundedDescription.text
  property alias primaryButton: primaryButton.text
  property alias secondaryButton: secondaryButton.text
  property alias specialComponent: loader.sourceComponent

  signal primaryButtonClicked
  signal secondaryButtonClicked

  width: window.width
  height: mainColumn.height
  edge: Qt.BottomEdge

  StyleV2 { id: styleV2 }

  Rectangle {
    color: roundedRect.color
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: 2 * radius
    anchors.topMargin: -radius
    radius: 20 * __dp
  }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: styleV2.whiteColor

    Column {
      id: mainColumn

      width: parent.width
      spacing: 20 * __dp
      leftPadding: 20 * __dp
      rightPadding: 20 * __dp
      bottomPadding: 20 * __dp

      Image {
        id: closeButton

        source: styleV2.closeButtonIcon
        anchors.right: parent.right
        anchors.rightMargin: 20 * __dp

        MouseArea {
          anchors.fill: parent
          onClicked: control.visible = false
        }
      }

      Image {
        id: picture

        anchors.horizontalCenter: parent.horizontalCenter
      }

      Text {
        id: title

        anchors.horizontalCenter: parent.horizontalCenter
        font: styleV2.t1
        width: parent.width - 2*20 * __dp
        color: styleV2.forestColor
        visible: text.length > 0
        horizontalAlignment: Text.AlignHCenter
      }

      Text {
        id: description

        anchors.horizontalCenter: parent.horizontalCenter
        font: styleV2.p5
        width: parent.width - 2*20 * __dp
        color: styleV2.nightColor
        visible: text.length > 0
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        lineHeight: 1.6
      }

      Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        visible: boundedDescription.text.length > 0
        width: parent.width - 2*20 * __dp
        height: boundedDescription.height
        radius: 16 * __dp
        color: styleV2.lightGreenColor

        Text {
          id: boundedDescription

          anchors.horizontalCenter: parent.horizontalCenter
          font: styleV2.p6
          width: parent.width
          color: styleV2.nightColor
          visible: text.length > 0
          horizontalAlignment: Text.AlignLeft
          wrapMode: Text.WordWrap
          lineHeight: 1.6
          padding: 20 * __dp
        }
      }

      Loader {
        id: loader

        anchors.horizontalCenter: parent.horizontalCenter
      }

      Item { width: 1; height: 1 }

      MMButton {
        id: primaryButton

        width: parent.width - 2*20 * __dp
        visible: text.length > 0

        onClicked: primaryButtonClicked()
      }

      MMButton {
        id: secondaryButton

        width: parent.width - 2*20 * __dp
        visible: text.length > 0
        transparent: true
        topPadding: 0

        onClicked: secondaryButtonClicked()
      }
    }
  }
}
