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
import Qt5Compat.GraphicalEffects

Item {
  id: root

  property alias imageSource: bngImage.source
  property string text: ""
  property bool textVisible: true

  readonly property int cornerRadius: __style.margin20
  readonly property int blurAmount: __style.margin30

  width: 300
  height: 300

  Rectangle {
    id: maskRect
    anchors.fill: parent
    radius: root.cornerRadius
    visible: false
  }

  Item {
    anchors.fill: parent
    layer.enabled: true
    layer.effect: OpacityMask {
      maskSource: maskRect
    }

    // A. Background Image
    Image {
      id: bngImage
      anchors.fill: parent
      fillMode: Image.PreserveAspectCrop
      smooth: true
      visible: true
    }

    Item {
      id: textContainer

      visible: root.textVisible

      anchors {
        bottom: parent.bottom
        left: parent.left
        right: parent.right
      }
      height: parent.height * 0.33

      clip: true

      ShaderEffectSource {
        id: effectSource
        sourceItem: bngImage

        sourceRect: Qt.rect(0, bngImage.height - textContainer.height, bngImage.width, textContainer.height)
        recursive: false
      }

      FastBlur {
        anchors.fill: parent
        source: effectSource
        radius: root.blurAmount
      }

      Rectangle {
        anchors.fill: parent
        color: __style.nightColor
        opacity: 0.35
      }

      Text {
        id: label
        text: root.text

        anchors.centerIn: parent
        width: parent.width - __style.margin32
        height: parent.height - __style.margin10

        color: __style.polarColor
        font {
          pixelSize: __style.margin10
          bold: true
          family: "Inter"
        }

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WrapAnywhere
        elide: Text.ElideRight
      }
    }
  }

  // add mouse area to go to the nex
}