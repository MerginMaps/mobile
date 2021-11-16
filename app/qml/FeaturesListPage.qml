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

import "./components"

Item {
  id: root

  signal addFeatureClicked()
  signal unlinkClicked()
  signal backButtonClicked()
  signal selectionFinished( var featureIds )

  property bool toolbarVisible: true
  property var toolbarButtons: [] // pass button names in list, see currently supported buttons below in toolbar

  property bool allowSearch: true
  property bool allowMultiselect: false
  property bool resetAfterSelection: true

  property var featuresModel: null
  property var preselectedFeatures: []

  property string pageTitle: featuresModel ? featuresModel.layer.name + " (" + featuresModel.layerFeaturesCount() + ")" : ""

  function finishSelection( featureIds )
  {
    if ( resetAfterSelection )
    {
      searchBar.deactivate()
    }
    selectionFinished( featureIds )
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
    if ( featuresModel )
    {
      if ( featuresModel.layerFeaturesCount() > featuresModel.featuresLimit )
        __inputUtils.showNotification( qsTr( "Showing only the first %1 features" ).arg( featuresModel.featuresLimit ) )
    }
  }

  Page {
    id: featuresPage

    anchors.fill: parent

    Keys.onReleased: {
      if ( event.key === Qt.Key_Back || event.key === Qt.Key_Escape ) {
        event.accepted = true;
        deactivateSearch()
        root.backButtonClicked()
      }
    }

    header: PanelHeader {
      id: featuresPageHeader

      width: parent.width
      height: InputStyle.rowHeightHeader

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

      visible: root.allowSearch
      allowTimer: true
      onSearchTextChanged: root.featuresModel.searchExpression = text
    }

    FeaturesList {
      id: listview

      width: parent.width
      height: allowSearch? parent.height - searchBar.height : parent.height

      y: allowSearch ? searchBar.height : 0

      clip: true

      showAdditionalInfo: root.state == "search"
      featuresModel: root.featuresModel
      allowMultiselect: root.allowMultiselect
      selectedFeatures: root.preselectedFeatures

      onFeatureClicked: root.finishSelection( featureId )
    }

    footer: FeaturesListPageToolbar {
      id: browseDataToolbar

      visible: root.toolbarVisible

      addButtonVisible: root.toolbarButtons.includes("add")
      doneButtonVisible: root.toolbarButtons.includes("done")
      unlinkButtonVisible: root.toolbarButtons.includes("unlink")

      onAddButtonClicked: addFeatureClicked()
      onDoneButtonClicked: root.finishSelection( listview.selectedFeatures )
      onUnlinkButtonClicked: root.unlinkClicked()
    }
  }
}
