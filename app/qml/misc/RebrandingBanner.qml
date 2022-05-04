/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtGraphicalEffects 1.14

import ".."
import "../components"

Item {
  id: root

  signal closed()

  height: 2 * InputStyle.rowHeight

  Rectangle {
    id: content

    anchors.fill: parent
    anchors.margins: InputStyle.tinyGap

    color: InputStyle.clrPanelMain
    radius: InputStyle.cornerRadius

    border.color: InputStyle.panelBackgroundDarker
    border.width: InputStyle.borderSize

    Column {
      anchors.topMargin: InputStyle.smallGap
      anchors.bottomMargin: InputStyle.tinyGap
      anchors.fill: parent

      Text {
        id: textItem

        width: parent.width
        height: parent.height / 3

        verticalAlignment: Text.AlignTop
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: InputStyle.fontPixelSizeNormal

        color: InputStyle.fontColor

        text: qsTr("%1Input%2 is becoming part of")
          .arg("<span style='font-weight:bold'>")
          .arg("</span>")

        textFormat: Text.RichText
        wrapMode: Text.WordWrap

        rightPadding: InputStyle.rowHeight / 4
      }

      Item {
        id: logoContainer

        width: parent.width
        height: parent.height * 2/3

        Image {
          id: logo

          height: parent.height * 0.7
          width: parent.width

          verticalAlignment: Image.AlignBottom

          source: InputStyle.mmLogoHorizontal

          sourceSize.width: width
          sourceSize.height: height
          fillMode: Image.PreserveAspectFit
        }
      }
    }

    MouseArea {
      anchors.fill: parent
      onClicked: Qt.openUrlExternally("https://www.lutraconsulting.co.uk/blog/2022/04/18/merginmaps?utm_campaign=rebrand-news&utm_medium=banner&utm_source=input-app")
    }

    Image {
      id: closeBtn

      anchors {
        top: parent.top
        topMargin: InputStyle.tinyGap
        right: parent.right
        rightMargin: InputStyle.tinyGap
      }

      source: InputStyle.noIcon
      visible: false

      width: InputStyle.closeBtnSize
      height: InputStyle.closeBtnSize

      sourceSize.width: width
      sourceSize.height: height
      fillMode: Image.PreserveAspectFit
    }

    ColorOverlay {
      anchors.fill: closeBtn
      source: closeBtn
      color: InputStyle.panelBackgroundDarker
    }

    MouseArea {
      anchors.fill: closeBtn
      onClicked: root.closed()
    }
  }
}
