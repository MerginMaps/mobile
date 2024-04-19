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

CheckBox {
  id: root

  property bool small: false

  property bool hasError: false

  property color emptyStateColor: __style.transparentColor

  implicitHeight: Math.max( textContent.implicitHeight, indicatorContent.height )

  topPadding: __style.margin4
  leftPadding: indicatorContent.width + __style.margin12
  rightPadding: 0
  bottomPadding: __style.margin4

  indicator: Rectangle {
    id: indicatorContent

    width: (root.small ? 16 : 24) * __dp
    height: width

    y: root.height / 2 - height / 2

    radius: __style.radius6

    color: ( root.checked && root.enabled ) ? __style.grassColor : emptyStateColor

    border.color: {
      if ( enabled ) {
        if ( checked ) {
          return __style.grassColor
        }

        if ( root.hasError ) {
          return __style.negativeColor
        }

        return __style.forestColor
      }

      return __style.mediumGreenColor
    }

    border.width: ( root.hovered ? 2.5 : 2 ) * __dp

    MMIcon {
      id: checkIndicator

      anchors.centerIn: parent

      source: __style.checkmarkIcon

      color: root.enabled ? __style.forestColor : __style.mediumGreenColor
      visible: root.checked
      size: root.small ? __style.icon16 : __style.icon24
    }
  }

  // Don't use MMText here, lineHeight does not work well with Text.RichText type
  contentItem: Text {
    id: textContent

    text: root.text

    font: __style.p5
    color: __style.nightColor

    textFormat: Text.RichText

    width: root.width - root.leftPadding

    verticalAlignment: Text.AlignVCenter

    lineHeight: 1.5 // mimic line height with factor
    wrapMode: Text.WordWrap

    onLinkActivated: function ( link ) {
      Qt.openUrlExternally( link )
    }
  }
}
