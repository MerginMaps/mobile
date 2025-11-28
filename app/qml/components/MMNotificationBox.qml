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

  signal buttonClicked()

  color: internal.bgndColor
  border.width: 1 * __dp
  border.color: internal.borderColor
  implicitHeight: dynamicContentGroup.implicitHeight + __style.margin20
  radius: __style.radius8

  Column {
    id: dynamicContentGroup

    width: parent.width - 2 * __style.margin20

    topPadding: __style.margin20
    leftPadding: __style.margin20
    

    spacing: __style.spacing12

    Text {
        id: titleText

        width: parent.width

        wrapMode: Label.Wrap

        font: __style.t4
        color: __style.nightColor

        lineHeight: __style.fontLineHeight24
        lineHeightMode: Text.FixedHeight

        verticalAlignment: Text.AlignVCenter
      }

      Text {
        id: descriptionText

        width: parent.width

        font: __style.p6
        color: __style.nightColor

        wrapMode: Label.Wrap

        lineHeight: __style.fontLineHeight24
        lineHeightMode: Text.FixedHeight

        verticalAlignment: Text.AlignVCenter

        visible: text ? true : false
      }

    MMButton {
      id: actionButton

      fontColor: internal.buttonFontColor
      bgndColor: internal.buttonBgndColor

      // reverse the color properties when the button is hovered
      fontColorHover: internal.buttonBgndColor
      bgndColorHover: internal.buttonFontColor

      size: MMButton.Sizes.Small

      onClicked: root.buttonClicked()
    }
  }

  QtObject {
    id: internal

    property color bgndColor: {
      switch (root.type) {
      case MMNotificationBox.Types.Warning:
        return __style.sandColor
      case MMNotificationBox.Types.Error:
        return __style.negativeLightColor
      }
    }

    property color borderColor: {
      switch (root.type) {
      case MMNotificationBox.Types.Warning:
        return __style.sunsetColor
      case MMNotificationBox.Types.Error:
        return __style.grapeColor
      }
    }

    property color buttonBgndColor: {
      switch(root.type) {
        case MMNotificationBox.Types.Warning:
          return __style.sunsetColor
        case MMNotificationBox.Types.Error:
          return __style.grapeColor
      }
    }

    property color buttonFontColor: {
      switch(root.type) {
        case MMNotificationBox.Types.Warning:
          return __style.earthColor
        case MMNotificationBox.Types.Error:
          return __style.roseColor
      }
    }
  }
}
