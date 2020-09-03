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

Item {
  id: root

  signal backButtonClicked()
  signal featureClicked( var featureId )
  signal addFeatureClicked()
  signal searchTextChanged( string text )

  property bool layerHasGeometry: true
  property string pageTitle: ""
  property int featuresCount: 0
  property int featuresLimit: 0
  property var featuresModel: null

  states: [
    State {
      name: "view"
      when: searchBar.text == ""
    },
    State {
      name: "search"
      when: searchBar.text != ""
    }
  ]
  Component.onCompleted: {
    if ( featuresCount > featuresLimit )
      __inputUtils.showNotification( qsTr( "Too many features in layer, showing first %1" ).arg( featuresLimit ) )
  }

  Page {
    id: featuresPage
    anchors.fill: parent

    header: PanelHeader {
      id: featuresPageHeader
      height: InputStyle.rowHeightHeader
      width: parent.width
      color: InputStyle.clrPanelMain
      rowHeight: InputStyle.rowHeightHeader
      titleText: pageTitle
      
      onBack: {
        searchBar.deactivate()
        root.backButtonClicked()
       }

      withBackButton: true
    }

    SearchBar {
      id: searchBar

      allowTimer: true
      onSearchTextChanged: {
        root.searchTextChanged( text )
      }
    }

    BrowseDataView {
      id: browseDataView
      width: parent.width
      height: parent.height - browseDataToolbar.height
      y: searchBar.height
      clip: true
      showAdditionalInfo: root.state == "search"
      featuresModel: root.featuresModel

      onFeatureClicked: root.featureClicked( featureId )
    }

    footer: BrowseDataToolbar {
      id: browseDataToolbar
      visible: !layerHasGeometry
      onAddButtonClicked: addFeatureClicked()
    }
  }
}
