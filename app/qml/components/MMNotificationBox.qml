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
    switch (type) {
    case MMNotificationBox.Types.Warning:
        return __style.sandColor
    case MMNotificationBox.Types.Error:
        return __style.negativeLightColor
    }
  }

  property color borderColor: {
    switch (type) {
    case MMNotificationBox.Types.Warning:
        return __style.sunsetColor
    case MMNotificationBox.Types.Error:
        return __style.grapeColor
    }
  }

  property color buttonBgndColor: {
    switch(type) {
      case MMNotificationBox.Types.Warning:
        return __style.sunsetColor
      case MMNotificationBox.Types.Error:
        return __style.grapeColor
    }
  }

  property color buttonFontColor: {
    switch(type) {
      case MMNotificationBox.Types.Warning:
        return __style.earthColor
      case MMNotificationBox.Types.Error:
        return __style.roseColor
    }
  }

  signal buttonClicked()

  color: bgndColor
  border.width: 1 * __dp
  border.color: borderColor
  implicitHeight: dynamicContentGroup.implicitHeight + 2 * __style.margin20
  radius: __style.radius12

    ColumnLayout {
    id: dynamicContentGroup

    width: parent.width - 2 * __style.margin20
    height: implicitHeight

    x: __style.margin20
    y: __style.margin20

    spacing: __style.spacing12

      Column {
        width:parent.width
        height: implicitHeight

        Text {
          id: titleText

          width: parent.width
          height: implicitHeight

          wrapMode: Label.Wrap

          font: __style.t3
          color: __style.nightColor

          lineHeight: __style.fontLineHeight24
          lineHeightMode: Text.FixedHeight

          verticalAlignment: Text.AlignVCenter
        }

        Text {
          id: descriptionText

          width: parent.width
          height: text ? implicitHeight : 0

          font: __style.p6
          color: __style.nightColor

          wrapMode: Label.Wrap

          lineHeight: __style.fontLineHeight24
          lineHeightMode: Text.FixedHeight

          verticalAlignment: Text.AlignVCenter
        }
      }
      MMButton {
        id: actionButton
        Layout.alignment: Qt.AlignRight
        fontColor: buttonFontColor
        bgndColor: buttonBgndColor
        // reverse the color properties when the button is hovered
        fontColorHover: buttonBgndColor
        bgndColorHover: buttonFontColor

        size: MMButton.Sizes.Small

        onClicked: root.buttonClicked()
      }
    }
}
