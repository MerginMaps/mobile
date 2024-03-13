/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

Item {
  id: root

  property var listRootObject: null

  // Usually MMScrollView.heigh - parent.implicitHeight
  // (parent is the main column of the scroll view)
  property real availableVerticalSpace: 0

  width: 1
  height: 0

  Connections {
    target: listRootObject

    function onHeightChanged() {
      root.recalculateHeight()
    }
  }

  Component.onCompleted: root.recalculateHeight()

  function recalculateHeight() {

    height = 0

    parent.forceLayout()

    let difference = availableVerticalSpace

    if ( difference > __style.margin20 ) {
      height = difference - parent.spacing
    }
    else {
      height = __style.margin20
    }
  }
}
