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
import "../Style.js" as Style
import "."

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

  Rectangle {
    color: roundedRect.color
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: 2 * radius
    anchors.topMargin: -radius
    radius: Style.commonSpacing
  }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: Style.white

    Column {
      id: mainColumn

      width: parent.width
      spacing: Style.commonSpacing
      leftPadding: Style.commonSpacing
      rightPadding: Style.commonSpacing
      bottomPadding: Style.commonSpacing

      Image {
        id: closeButton

        source: Style.closeButtonIcon
        anchors.right: parent.right
        anchors.rightMargin: Style.commonSpacing

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
        font: Qt.font(Style.t1)
        width: parent.width - 2*Style.commonSpacing
        color: Style.forest
        visible: text.length > 0
        horizontalAlignment: Text.AlignHCenter
      }

      Text {
        id: description

        anchors.horizontalCenter: parent.horizontalCenter
        font: Qt.font(Style.p5)
        width: parent.width - 2*Style.commonSpacing
        color: Style.night
        visible: text.length > 0
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        lineHeight: 1.6
      }

      Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        visible: boundedDescription.text.length > 0
        width: parent.width - 2*Style.commonSpacing
        height: boundedDescription.height
        radius: 16 * __dp

        color: Style.lightGreen

        Text {
          id: boundedDescription

          anchors.horizontalCenter: parent.horizontalCenter
          font: Qt.font(Style.p6)
          width: parent.width
          color: Style.night
          visible: text.length > 0
          horizontalAlignment: Text.AlignLeft
          wrapMode: Text.WordWrap
          lineHeight: 1.6
          padding: Style.commonSpacing
        }
      }

      Loader {
        id: loader

        anchors.horizontalCenter: parent.horizontalCenter
      }

      Item { width: 1; height: 1 }

      MMButton {
        id: primaryButton

        width: parent.width - 2*Style.commonSpacing
        visible: text.length > 0

        onClicked: primaryButtonClicked()
      }

      MMButton {
        id: secondaryButton

        width: parent.width - 2*Style.commonSpacing
        visible: text.length > 0
        transparent: true
        topPadding: 0

        onClicked: secondaryButtonClicked()
      }
    }
  }
}
