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
  property real gap: __style.margin12
  property real popupWidth: 155 * __dp

  // Adjust the width padding, for some reason this helps to removing the clipping effects.
  property real widthPadding: 200
  // small static cushion against rounding - or else clipping on the right while shrinking
  property real allSafety: 25 * __dp

  //For controlling the jittering
  property real lagFix: 25 * __dp   // extra headroom before expanding to ALL
  property int  debounceMs: 20          // debounce while resizing- used inside Timer

  // IMPORTANT: “one extra button” worthy of space before expanding to ALL
  property bool reserveOneMore: true

  readonly property var _visible: actions.filter(a => a && a.visible !== false)
  readonly property int _count: _visible.length

  readonly property var modeEnum: ({ pair: 0, all: 1 })
  property int _mode: modeEnum.pair
  readonly property bool _all:  _mode === modeEnum.all
  readonly property bool _pair: _mode === modeEnum.pair

  property var  _widths: []
  property real _hiddenW: 0   // (only for popup anchor)

  MMComponents.MMRoundButton {
    id: moreButton
    visible: false; opacity: 0
    iconSource: __style.moreIcon
    bgndColor: __style.lightGreenColor
    Component.onCompleted: { root._hiddenW = implicitWidth; _recalcSoon() }
    onImplicitWidthChanged: { root._hiddenW = implicitWidth; _recalcSoon() }
  }

  Instantiator {
    id: measurer
    model: root._visible
    delegate: MMComponents.MMButton {
      visible: false; opacity: 0
      text: modelData.label
      iconSourceLeft: modelData.icon
      type: modelData.style === "Secondary"
            ? MMComponents.MMButton.Secondary
            : MMComponents.MMButton.Primary
      Component.onCompleted: root._storeWidth(index, implicitWidth)
      onImplicitWidthChanged: root._storeWidth(index, implicitWidth)
    }
    onObjectRemoved: _recalcSoon()
  }

  // RECOMPUTE e s
  Timer { id: debounce; interval: debounceMs; onTriggered: decide() }
  function _recalcSoon() { debounce.restart() }

  onWidthChanged:         {
                            _recalcSoon();
                            if (_all)
                              collapseGuard.start()
                          }
  onActionsChanged:       {
                            _widths = [];
                            _recalcSoon()
                          }
  onGapChanged:             _recalcSoon()
  onWidthPaddingChanged:    _recalcSoon()
  onAllSafetyChanged:       _recalcSoon()
  onLagFixChanged:          _recalcSoon()
  onReserveOneMoreChanged:  _recalcSoon()
  Component.onCompleted:    _recalcSoon()

  implicitHeight: Math.max(allRow.implicitHeight, pairRow.implicitHeight)
  clip: true

  Row {
    id: allRow
    visible: root._all
    spacing: root.gap
    anchors.left: parent.left
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
    spacing: root.gap
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

  // collapse if ALL overflows after a shrink
  Timer {
    id: collapseGuard
    interval: 0
    repeat: false
    onTriggered: {
      if (root._all && allRow.width > root.width) _setMode(root.modeEnum.pair)
    }
  }

  ///////////////////////////////////
  function _storeWidth(i, w) {
    const arr = _widths.slice()
    arr[i] = Math.ceil(w || 0)
    _widths = arr
    _recalcSoon()
  }

  function _ready() {
    if (_count <= 0) return false
    if (_widths.length !== _count) return false
    for (let i = 0; i < _count; ++i) if (!_widths[i] || _widths[i] <= 0) return false
    return true
  }

  function _sumWidths() {
    let s = 0
    for (let i = 0; i < _count; ++i) s += _widths[i]
    return s
  }

  function _maxWidth() {
    let m = 0
    for (let i = 0; i < _count; ++i) if (_widths[i] > m) m = _widths[i]
    return m
  }

  // width required if ALL inline
  function _needAllBase() {
    const gaps = _count > 1 ? (_count - 1) * gap : 0
    return Math.ceil(_sumWidths() + gaps + Math.max(0, widthPadding) + Math.max(0, allSafety))
  }

  //////////////////////////////

  function _setMode(m) {
    if (_mode === m) return
    _mode = m
    if (_mode === modeEnum.all && popup.opened) popup.close()
    // after switching to ALL double-check once layout setles
    if (_mode === modeEnum.all) collapseGuard.start()
  }

  function decide() {
    if (_count <= 1) { _setMode(modeEnum.all); return }
    if (!_ready())   { _setMode(modeEnum.pair); return }

    const avail    = Math.max(0, Math.floor(root.width))
    const needAll  = _needAllBase()
    const extraOne = (reserveOneMore && _count > 0) ? (_maxWidth() + (_count > 0 ? gap : 0)) : 0

    if (_mode === modeEnum.all) {
      // Collapse as soon as ALL no longer fits -> we also have the post-layout guard
      if (needAll > avail) _setMode(modeEnum.pair)
    } else {
      // Expand only when ALL + extra one + lagFix fits comfortably.
      if (needAll + extraOne + Math.max(0, lagFix) <= avail)
        _setMode(modeEnum.all)
    }
  }
}
