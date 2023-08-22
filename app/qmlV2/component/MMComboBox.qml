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
import "../Style.js" as Style

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
      source: popup.visible ? Style.arrowUpIcon : Style.arrowDownIcon
      color: control.enabled ? Style.forest : Style.mediumGreen
      x: combobox.width - width - combobox.rightPadding
      anchors.verticalCenter: parent.verticalCenter
    }

    contentItem: Text {
      leftPadding: 20
      rightPadding: combobox.indicator.width + combobox.spacing

      text: combobox.displayText
      font: Qt.font(Style.p4)
      color: control.enabled ? Style.night : Style.mediumGreen
      verticalAlignment: Text.AlignVCenter
      elide: Text.ElideRight
    }

    background: Rectangle {
      color: (errorMsg.length > 0 || warningMsg.length > 0) ? Style.errorBgInputColor : Style.white
      border.color: (combobox.activeFocus || combobox.hovered) ? (errorMsg.length > 0 ? Style.negative :
                                                                                        warningMsg.length > 0 ? Style.warning :
                                                                                                                Style.forest) : "transparent"
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
        border.color: Style.forest
        radius: 2
      }
    }

    delegate: ItemDelegate {
      id: delegate

      width: popup.width
      height: 30 * __dp
      contentItem: Text {
        text: modelData
        color: combobox.highlightedIndex === index ? Style.grass : Style.forest
        font: Qt.font(Style.p4)
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
      }
      background: Rectangle {
        border.color: Style.white
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

        source: visible ? Style.errorIcon : ""
        color: errorMsg.length > 0 ? Style.negative : Style.warning
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
      Text {
        text: errorMsg.length > 0 ? errorMsg : warningMsg
        font: Qt.font(Style.t4)
        wrapMode: Text.WordWrap
        width: messageItem.width - msgRow.spacing - msgIcon.width
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
    }
  }
}
