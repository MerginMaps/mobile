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
import Qt5Compat.GraphicalEffects
import lc 1.0
import "." // import InputStyle singleton
import "./components"
import "./misc"

Page {
  id: root

  signal back

  visible: false
  property real rowHeight: InputStyle.rowHeight

  header: PanelHeaderV2 {
    width: root.width
    headerTitle: qsTr( "Layers attribution" )
    onBackClicked: root.back()
  }

  Text {
    id: attributionText

    anchors.fill: parent
    textFormat: Text.RichText
    wrapMode: Text.WordWrap
    text: __inputUtils.layersAttribution(__activeProject.qgsProject)
  }
}
