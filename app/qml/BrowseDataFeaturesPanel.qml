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
  signal featureClicked( var featureIds )
  signal addFeatureClicked()
  signal searchTextChanged( string text )

  property bool layerHasGeometry: true
  property bool allowMultiselect: false
  property string layerName: ""
  property var featuresModel: null
  property int featuresCount: featuresModel ? featuresModel.featuresCount : 0
  property int featuresLimit: featuresModel ? featuresModel.featuresLimit : 0
  property string pageTitle: layerName + " (" + featuresCount + ")"

  property var deactivateSearch: function deactivateSearch() {
    searchBar.deactivate()
  }

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
      __inputUtils.showNotification( qsTr( "Showing only the first %1 features" ).arg( featuresLimit ) )
  }

  onVisibleChanged: {
    // On Android, due to a Qt bug, we need to call deactivate again on page close to clear text search
    if ( !visible && __androidUtils.isAndroid )
      searchBar.deactivate()
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
      allowMultiselect: root.allowMultiselect

      onFeatureClicked: root.featureClicked( featureId )
    }

    footer: BrowseDataToolbar {
      id: browseDataToolbar
      visible: !layerHasGeometry || allowMultiselect
      addButtonVisible: !layerHasGeometry
      doneButtonVisible: allowMultiselect

      onAddButtonClicked: addFeatureClicked()
      onDoneButtonClicked: root.featureClicked( browseDataView.selectedIds )
    }
  }
}
