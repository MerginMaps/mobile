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

Column {
  id: root

  required property string link

  spacing: __style.margin12

  Image {
    id: illustration
    source: __style.externalGpsRedImage
    anchors.horizontalCenter: parent.horizontalCenter
  }

  Text {
    id: titleItem

    width: parent.width - 2 * __style.pageMargins
    x: __style.pageMargins
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter

    font: __style.t1
    color: __style.forestColor
    text: qsTr( "Connecting to external receivers via bluetooth is not supported" )
    wrapMode: Text.Wrap
  }

  Text {
    id: descItem

    width: parent.width - 2 * __style.pageMargins
    x: __style.pageMargins
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter

    font: __style.p5
    color: __style.nightColor
    text: qsTr( "This function is not available on iOS. " +
                "Your hardware vendor may provide a custom " +
               "app that connects to the receiver and sets position. " +
               "Mergin Maps will still think it is the internal GPS of " +
               "your phone/tablet.")

    wrapMode: Text.Wrap
    textFormat: Text.RichText
    onLinkActivated: function( link ) {
      Qt.openUrlExternally( link )
    }
  }

  Text {
    id: linkItem

    width: parent.width - 2 * __style.pageMargins
    x: __style.pageMargins
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter

    font: __style.t3
    color: __style.forestColor
    text: qsTr( "%1Learn more%2" )
    .arg( "<a style=\"text-decoration: underline; color:" + __style.forestColor + ";\" href='" + root.link + "'>" )
    .arg( "</a>" )

    textFormat: Text.RichText
    onLinkActivated: function( link ) {
      Qt.openUrlExternally( link )
    }
  }
}
