/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.0
import QtQuick.Controls 2.12
import QgsQuick 0.1 as QgsQuick
import lc 1.0

Item {
  id: root

  signal backButtonClicked()
  signal layerClicked( var layerId )

  Page {
    id: layersListPage
    anchors.fill: parent

    header: PanelHeader {
      id: layersPageHeader
      height: InputStyle.rowHeightHeader
      width: parent.width
      color: InputStyle.clrPanelMain
      rowHeight: InputStyle.rowHeightHeader
      titleText: qsTr("Layers")
      
      onBack: root.backButtonClicked()
      withBackButton: true
    }
    
    LayerList {
        implicitHeight: layersListPage.height - layersPageHeader.height
        width: parent.width
        model: __browseDataLayersModel

        cellWidth: width
        cellHeight: InputStyle.rowHeight
        borderWidth: 1
        highlightingAllowed: false
        noLayersText: qsTr("No identifiable layers in the project!")

        onListItemClicked: {
          layerClicked( layerId )
        }
    }
  }
}
