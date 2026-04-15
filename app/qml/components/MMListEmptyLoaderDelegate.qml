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

  property bool isLoading: false

  height: root.isLoading
    ? busyIndicator.height + 2 * __style.margin12
    : noItemsText.implicitHeight + 2 * __style.margin20

  MMText {
    id: noItemsText

    anchors.centerIn: parent

    visible: !root.isLoading

    text: qsTr( "No items" )
    font: __style.p5
    color: __style.mediumGreyColor
  }

  MMBusyIndicator {
    id: busyIndicator

    anchors.centerIn: parent

    running: root.isLoading
  }
}
