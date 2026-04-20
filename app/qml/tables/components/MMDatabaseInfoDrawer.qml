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

import mm 1.0 as MM

import "../../components"

/*
 * MMDatabaseInfoDrawer — database information drawer.
 * Extracted from CreateTableDialog.qml to follow the reusable components pattern.
 */

MMDrawer {
  id: root

  property string infoText: ""

  drawerHeader.title: qsTr("Información de Base de Datos")

  drawerContent: MMScrollView {
    width: parent.width
    height: Math.min(400 * __dp, (ApplicationWindow.window?.height ?? 600) * 0.5)

    MMText {
      width: parent.width
      text: root.infoText
      font: __style.p6
      color: __style.nightColor
      wrapMode: Text.WordWrap
    }
  }
}
