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
import QtQuick.Layouts

Rectangle {
  id: root

  property alias title: titleText.text
  property alias description: descriptionText.text

  property int descriptionMaxLines: 2

  property string linkText

  property color textColor: __style.deepOceanColor
  property alias imageSource: severityImage.source

  signal clicked()

  // Please set width
  // Height is calculated automatically based on the text wrapping

  implicitHeight: dynamicContentGroup.height + 2 * __style.margin20

  color: __style.polarColor
  radius: __style.radius12

  RowLayout {
    id: dynamicContentGroup

    width: parent.width - 2 * __style.margin20
    height: Math.max( severityImage.implicitHeight, titleText.implicitHeight + descriptionText.implicitHeight )

    x: __style.margin20
    y: __style.margin20

    spacing: __style.spacing12

    Image {
      id: severityImage

      Layout.preferredHeight: 50 * __dp
      Layout.preferredWidth: 50 * __dp
      Layout.alignment: Qt.AlignVCenter

      sourceSize.width: 50 * __dp
      sourceSize.height: 50 * __dp
    }

    Column {

      Layout.fillWidth: true
      Layout.preferredHeight: titleText.implicitHeight + descriptionText.height
      Layout.alignment: Qt.AlignVCenter

      Text {
        id: titleText

        width: parent.width
        height: implicitHeight

        elide: Text.ElideRight

        wrapMode: descriptionText.text ? Text.NoWrap : Text.Wrap
        maximumLineCount: descriptionText.text ? 1 : 2

        font: __style.t3
        color: root.textColor

        lineHeight: __style.fontLineHeight24
        lineHeightMode: Text.FixedHeight

        verticalAlignment: Text.AlignVCenter
      }

      Text {
        id: descriptionText

        width: parent.width
        height: text ? implicitHeight : 0

        font: __style.p6
        color: root.textColor

        wrapMode: Label.Wrap
        maximumLineCount: descriptionMaxLines
        elide: Text.ElideRight

        lineHeight: __style.fontLineHeight24
        lineHeightMode: Text.FixedHeight

        verticalAlignment: Text.AlignVCenter
      }
    }

    Text {

      Layout.preferredWidth: paintedWidth
      Layout.preferredHeight: paintedHeight

      Layout.alignment: Qt.AlignVCenter

      text: root.linkText
      visible: text

      font: __style.t3
      color: __style.forestColor

      lineHeight: __style.fontLineHeight24
      lineHeightMode: Text.FixedHeight

      verticalAlignment: Text.AlignVCenter
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: function( mouse ) {
      mouse.accepted = true
      root.clicked()
    }
  }
}
