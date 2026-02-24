/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick
import QtQuick.Effects

Item {
  id: root

  property alias imageSource: bngImage.photoUrl
  property string text: ""
  property bool textVisible: true
  property int size: 120

  signal clicked(url path)

  height: size
  width: size

  layer.enabled: true
  layer.effect: MultiEffect {
    maskEnabled: true
    maskSource: photoMask
    autoPaddingEnabled: false
  }

  Rectangle {
    id: photoMask
    width: root.width
    height: root.height
    radius: 20 * __dp
    visible: false
    layer.enabled: true
  }

  MMPhoto {
    id: bngImage
    anchors.fill: parent
    fillMode: Image.PreserveAspectCrop
    autoTransform: true
    smooth: true
  }

  MultiEffect{
    id: blurFooter
    height: parent.height * 0.33
    visible: root.textVisible
    anchors {
      left: parent.left
      right: parent.right
      bottom: parent.bottom
    }

    source: ShaderEffectSource {
      sourceItem: bngImage
      sourceRect: Qt.rect(0, bngImage.height - bngImage.height * 0.33, bngImage.width,
        bngImage.height * 0.33)
      recursive: false
    }

    autoPaddingEnabled: false
    blurEnabled: true
    blur: 0.8
  }

  // tint overlay
  Rectangle {
    anchors.fill: blurFooter
    color: __style.nightColor
    opacity: 0.35
    visible: root.textVisible
  }

  Text {
    visible: root.textVisible
    text: root.text
    width: root.width - 2 * (root.size * 0.15)
    anchors.centerIn: blurFooter

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

  MMSingleClickMouseArea {
    anchors.fill: parent
    onSingleClicked: root.clicked(root.imageSource)
  }
}
