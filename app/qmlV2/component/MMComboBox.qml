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
import ".."

Item {
  id: control

  property alias model: combobox.model
  property string warningMsg
  property string errorMsg

  signal activated(currentValue: string)

  width: 280 * __dp
  height: combobox.height + messageItem.height

  ComboBox {
    id: combobox

    width: parent.width
    height: 40 * __dp

    indicator: MMIcon {
      source: popup.visible ? StyleV2.arrowUpIcon : StyleV2.arrowDownIcon
      color: control.enabled ? StyleV2.forestColor : StyleV2.mediumGreenColor
      x: combobox.width - width - combobox.rightPadding
      anchors.verticalCenter: parent.verticalCenter
    }

    contentItem: Text {
      leftPadding: 20
      rightPadding: combobox.indicator.width + combobox.spacing

      text: combobox.displayText
      font: StyleV2.p4
      color: control.enabled ? StyleV2.nightColor : StyleV2.mediumGreenColor
      verticalAlignment: Text.AlignVCenter
      elide: Text.ElideRight
    }

    background: Rectangle {
      color: (errorMsg.length > 0 || warningMsg.length > 0) ? StyleV2.errorBgInputColor : StyleV2.whiteColor
      border.color: (combobox.activeFocus || combobox.hovered) ? (errorMsg.length > 0 ? StyleV2.negativeColor :
                                                                                        warningMsg.length > 0 ? StyleV2.warningColor :
                                                                                                                StyleV2.forestColor) : StyleV2.transparentColor
      border.width: enabled ? (combobox.activeFocus ? 2*__dp : 1*__dp) : 0
      radius: parent.height
    }

    popup: Popup {
      id: popup

      y: combobox.height - 1
      x: 20 * __dp
      width: combobox.width - 2 * x
      implicitHeight: contentItem.implicitHeight
      padding: 1

      contentItem: ListView {
        clip: true
        implicitHeight: contentHeight
        model: combobox.popup.visible ? combobox.delegateModel : null
        currentIndex: combobox.highlightedIndex
        ScrollIndicator.vertical: ScrollIndicator { }
      }

      background: Rectangle {
        border.color: StyleV2.forestColor
        radius: 2
      }
    }

    delegate: ItemDelegate {
      id: delegate

      width: popup.width
      height: 30 * __dp
      contentItem: Text {
        text: modelData
        color: combobox.highlightedIndex === index ? StyleV2.grassColor : StyleV2.forestColor
        font: StyleV2.p4
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
      }
      background: Rectangle {
        border.color: StyleV2.whiteColor
      }
    }

    onActivated: control.activated(combobox.currentValue)
  }

  Item {
    id: messageItem

    width: parent.width
    anchors.left: parent.left
    anchors.top: combobox.bottom
    anchors.topMargin: 6 * __dp
    height: msgRow.height

    Row {
      id: msgRow

      spacing: 4 * __dp

      MMIcon {
        id: msgIcon

        source: visible ? StyleV2.errorIcon : ""
        color: errorMsg.length > 0 ? StyleV2.negativeColor : StyleV2.warningColor
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
      Text {
        text: errorMsg.length > 0 ? errorMsg : warningMsg
        font: StyleV2.t4
        wrapMode: Text.WordWrap
        width: messageItem.width - msgRow.spacing - msgIcon.width
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
    }
  }
}
