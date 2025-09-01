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

  // tiny lag smoother to avoid edge flip-flop- tweak if needed
  property bool fitsInline: true

  implicitHeight: (fitsInline ? allRow.implicitHeight : pairRow.implicitHeight)

  // ReEvaluate when width changes dynamically
  onWidthChanged: {
    const need = allRow.implicitWidth
    if (fitsInline) {
      if (need > width) {
        fitsInline = false
      }
    }
    else {
      if (need + 2 <= width) {
        fitsInline = true
      }
    }
  }
  onActionsChanged: {
    const need = allRow.implicitWidth
    if (fitsInline) {
      if (need > width) {
        fitsInline = false
      }
    }
    else {
      if (need + 2 <= width) {
        fitsInline = true
      }
    }
  }
  Connections {
    target: allRow
    function onImplicitWidthChanged() {
      const need = allRow.implicitWidth
      if (root.fitsInline) {
        if (need > root.width) {
          root.fitsInline = false
        }
      }
      else {
        if (need + 2 <= root.width) {
          root.fitsInline = true
        }
      }
    }
  }

  Row {
    id: allRow
    anchors.left: parent.left
    spacing: __style.margin12
    // measure content only; don't bind to parent width
    clip: true
    height: fitsInline ? allRow.implicitHeight : 0
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
    visible: !fitsInline && _count > 0
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
          const a = _visible[0]
          if (a && typeof a.onTrigger === "function") a.onTrigger()
        }
      }
    }

    MMComponents.MMRoundButton {
      id: hidden
      iconSource: __style.moreIcon
      bgndColor: __style.lightGreenColor
      onClicked: popup.opened ? popup.close() : popup.open()
    }
  }

  // Popup -> everything except the first
  MMComponents.MMPopup {
    id: popup
    parent: hidden
    x: parent.width - width
    y: parent.height + __style.margin12
    width: 155 * __dp

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
