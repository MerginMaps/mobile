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
  property alias buttonText: actionButton.text

  enum Types { Warning, Error }
  property int type: MMNotificationBox.Types.Warning

  property color bgndColor: {
    if ( type === MMNotificationBox.Types.Warning ) return __style.sandColor
    if ( type === MMNotificationBox.Types.Error ) return __style.negativeLightColor
  }

  property color borderColor: {
    if ( type === MMNotificationBox.Types.Warning ) return __style.sunsetColor
    if ( type === MMNotificationBox.Types.Error ) return __style.grapeColor
  }

  signal buttonClicked()

  property color textColor: __style.nightColor

  color: bgndColor
  border.width: 1 * __dp
  border.color: borderColor
  implicitHeight: dynamicContentGroup.implicitHeight + 2 * __style.margin20
  radius: __style.radius12

  RowLayout {
    id: dynamicContentGroup

    width: parent.width - 2 * __style.margin20
    height: implicitHeight

    x: __style.margin20
    y: __style.margin20

    spacing: __style.spacing12

    Column {

      Layout.fillWidth: true
      Layout.preferredHeight: implicitHeight
      spacing: 12 * __dp

      Column {
        width:parent.width
        height: implicitHeight

        Text {
          id: titleText

          width: parent.width
          height: implicitHeight

          wrapMode: Label.Wrap

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

          lineHeight: __style.fontLineHeight24
          lineHeightMode: Text.FixedHeight

          verticalAlignment: Text.AlignVCenter
        }
      }

      MMButton {
        id: actionButton

        fontColor: root.type === MMNotificationBox.Types.Warning ? __style.earthColor : __style.roseColor
        bgndColor: root.type === MMNotificationBox.Types.Warning ? __style.sunsetColor : __style.grapeColor
        fontColorHover: root.type === MMNotificationBox.Types.Warning ? __style.sunsetColor : __style.grapeColor
        bgndColorHover: root.type === MMNotificationBox.Types.Warning ? __style.earthColor : __style.roseColor

        size: MMButton.Sizes.Small

        onClicked: root.buttonClicked()
      }
    }
  }
}
