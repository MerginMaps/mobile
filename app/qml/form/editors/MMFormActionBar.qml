/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "../../components" as MMComponents

Item {
  id: root

  // actions = [{ label, icon, style: "Primary"|"Secondary", visible, onTrigger }]
  property var  actions: []

  readonly property var _visible: actions.filter(a => a && a.visible !== false)
  readonly property int _count: _visible.length

  // tiny lag smoother to avoid edge flip-flop- tweak if neede
  readonly property bool fitsInline: allRow.implicitWidth + 1 <= width

  Item {
    anchors.left: parent.left
    anchors.right: parent.right
    height: Math.max(allRow.implicitHeight, pairRow.implicitHeight)

    Row {
      id: allRow
      visible: root.fitsInline
      anchors.left: parent.left
      spacing: __style.margin12
      width: childrenRect.width

      Repeater {
        model: root._visible
        delegate: MMComponents.MMButton {
          text: modelData.label
          iconSourceLeft: modelData.icon
          type: modelData.style === "Secondary" ? MMComponents.MMButton.Secondary : MMComponents.MMButton.Primary
          onClicked: if (typeof modelData.onTrigger === "function") modelData.onTrigger()
        }
      }
    }

    RowLayout {
      id: pairRow
      visible: root._pair && root._count > 0
      spacing: __style.margin12
      anchors.left: parent.left
      anchors.right: parent.right

      MMComponents.MMButton {
        Layout.fillWidth: true
        text:  root._visible[0].label
        iconSourceLeft: root._visible[0].icon
        type: (root._visible[0].style === "Secondary") ? MMComponents.MMButton.Secondary : MMComponents.MMButton.Primary
        onClicked: {
          const a = root._visible[0]
          if (a && typeof a.onTrigger === "function") a.onTrigger()
        }
      }

      MMComponents.MMRoundButton {
        id: hidden
        iconSource: __style.moreIcon
        bgndColor: __style.lightGreenColor
        onClicked: popup.opened ? popup.close() : popup.open()
      }
    }
  }

  // Popup -> everything except the first
  MMComponents.MMPopup {
    id: popup
    parent: hidden
    x: parent.width - width
    y: parent.height + __style.margin12
    width: menuColumn.width

    transformOrigin: Item.TopRight

    contentItem: Column {
      spacing: 0
      width: root.popupWidth

      Repeater {
        model: root._count > 1 ? root._visible.slice(1) : []
        delegate: MMComponents.MMListDelegate {
          width: parent.width
          text: modelData.label
          verticalSpacing: __style.margin6
          hasLine: index < (model.count - 1)
          onClicked: {
            popup.close()
            if (typeof modelData.onTrigger === "function") modelData.onTrigger()
          }
          leftContent: [
            Item {
              width: 28 * __dp; height: 28 * __dp
              anchors.verticalCenter: parent.verticalCenter
              MMComponents.MMIcon {
                anchors.centerIn: parent
                source: modelData.icon
                width: 20 * __dp; height: 20 * __dp
                color: __style.nightColor
              }
            }
          ]
        }
      }
    }
  }
}
