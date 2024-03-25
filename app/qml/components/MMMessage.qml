/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components"

/**
 * Component that shows
 *   Image,
 *   Title (e.g. Select Layer)
 *   Description (e.g. No layers available, set them in QGIS)
 *   Link (e.g. Learn More with link to merginmaps.com/docs)
 * all centered
 */

Column {
  id: root

  property alias image: illustration.source
  property string title
  property string description

  property string linkText: qsTr( "Learn more" )
  property string link

  spacing: 0

  Image {
    id: illustration
    anchors.horizontalCenter: parent.horizontalCenter
    sourceSize.width: 140 * __dp
  }

  MMListSpacer { height: __style.margin20 }

  MMText {
    id: titleItem

    width: parent.width

    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter

    visible: root.title

    font: __style.t1
    color: __style.forestColor
    text: root.title
    wrapMode: Text.Wrap
  }

  MMListSpacer { height: __style.margin12 }

  MMText {
    id: descItem

    width: parent.width

    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter

    visible: root.description

    font: __style.p5
    color: __style.nightColor
    text: root.description

    wrapMode: Text.Wrap
    textFormat: Text.RichText
    onLinkActivated: function( link ) {
      Qt.openUrlExternally( link )
    }
  }

  MMListSpacer { height: __style.margin12 }

  Text {
    id: linkItem

    width: parent.width

    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter

    visible: root.linkText && root.link

    font: __style.t3
    color: __style.forestColor
    text: "<a style=\"text-decoration: underline; color:" + __style.forestColor + ";\" href='" + root.link + "'>" + root.linkText + "</a>"

    textFormat: Text.RichText

    onLinkActivated: function( link ) {
      Qt.openUrlExternally( link )
    }
  }
}
