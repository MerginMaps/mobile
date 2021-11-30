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

import lc 1.0
import "./components"

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
        noLayersText: qsTr("No layers have been configured to allow browsing their features. See %1how to modify your project%2.")
                      .arg("<a href='"+ __inputHelp.howToEnableBrowsingDataLink +"'>")
                      .arg("</a>")

        onListItemClicked: {
          layerClicked( layerId )
        }
    }
  }
}
