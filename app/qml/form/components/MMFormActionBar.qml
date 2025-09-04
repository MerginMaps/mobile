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

  // actions = [{ label, icon, visible, onTrigger }]
  property var  actions: []

  readonly property var _visible: actions.filter(a => a && a.visible !== false)
  readonly property int _count: _visible.length

  implicitHeight: allRow.implicitHeight || pairRow.implicitHeight

  Row {
    id: allRow
    anchors.left: parent.left
    spacing: __style.margin12
    // measure content only; don't bind to parent width
    clip: true
    visible: implicitWidth < root.width
    width: childrenRect.width

    Repeater {
      model: root._visible
      delegate: MMComponents.MMButton {
        text: modelData.label
        iconSourceLeft: modelData.icon
        type: (index === 0)
                    ? MMComponents.MMButton.Primary
                    : MMComponents.MMButton.Secondary
        onClicked: if (typeof modelData.onTrigger === "function") modelData.onTrigger()
      }
    }
  }

  RowLayout {
    id: pairRow
    visible: !allRow.visible && _count > 0
    anchors.left: parent.left
    anchors.right: parent.right
    spacing: __style.margin12

    MMComponents.MMButton {
      Layout.fillWidth: true
      text:  _count > 0 ? _visible[0].label : ""
      iconSourceLeft: _count > 0 ? _visible[0].icon : ""
      type: (_count > 0 && _visible[0].style === "Secondary")
                  ? MMComponents.MMButton.Secondary
                  : MMComponents.MMButton.Primary
      onClicked: {
        if (_count > 0) {
          const actionFunction = _visible[0]
          if (actionFunction && typeof actionFunction.onTrigger === "function") actionFunction.onTrigger()
        }
      }
    }

    MMComponents.MMRoundButton {
      id: hidden
      visible: !allRow.visible && _count > 1
      iconSource: __style.moreIcon
      bgndColor: __style.lightGreenColor
      onClicked: popup.opened ? popup.close() : popup.open()
    }
  }

  MMComponents.MMPopup {
    id: popup
    parent: hidden
    x: parent.width - width
    property int heightCalculate: hidden.mapToItem(Overlay.overlay, 0, 0).y + hidden.height + __style.margin12 + implicitHeight
    y: (heightCalculate > ApplicationWindow.window.height)
       ? (-implicitHeight - __style.margin12)  //Switches above the button
       : (parent.height + __style.margin12) //remains below the button

    width: 155 * __dp

    //transformOrigin: Item.TopRight

    contentItem: Column {
      spacing: 0
      width: 155 * __dp

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
