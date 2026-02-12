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

  property alias imageSource: bngImage.photoUrl
  property string text: ""
  property bool textVisible: true

  signal clicked(string path)

  width: 120
  height: 120

  Rectangle {
    id: maskRect
    anchors.fill: parent
    radius: __style.margin20
    visible: false
  }

  Item {
    anchors.fill: parent
    layer.enabled: true
    layer.effect: OpacityMask {
      maskSource: maskRect
    }

    MMPhoto {
      id: bngImage
      anchors.fill: parent
      fillMode: Image.PreserveAspectCrop
      autoTransform: true
      smooth: true
    }

    // the footer of the card
    // contains the text to display over the blurred part of the image
    FastBlur {
      id: footer
      visible: root.textVisible

      anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
      }
      height: parent.height * 0.33

      radius: 20
      source: ShaderEffectSource {
        sourceItem: bngImage
        sourceRect: Qt.rect(0, bngImage.height - footer.height, bngImage.width,
                            footer.height)
        recursive: false
      }

      // Tint Overlay (Child of FastBlur)
      Rectangle {
        anchors.fill: parent
        color: __style.nightColor
        opacity: 0.35
      }

      Text {
        text: root.text
        width: root.width - 2 * __style.margin16
        anchors.centerIn: parent

        color: __style.polarColor
        font: __style.t5

        lineHeightMode: Text.FixedHeight
        lineHeight: __style.margin16
        maximumLineCount: 2

        horizontalAlignment: Text.AlignHCenter | Text.AlignJustify
        verticalAlignment: Text.AlignVCenter
        
        wrapMode: Text.WordWrap
        elide: Text.ElideRight
      }
    }
  }

  MMSingleClickMouseArea {
    anchors.fill: parent
    onSingleClicked: root.clicked(root.imageSource)
  }
}
