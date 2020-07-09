import QtQuick 2.0
import QtQuick.Controls 2.12
import lc 1.0

/*
 * BrowseDataPanel should stay a logic component, please do not combine UI here
 */

Item {
  id: root
  visible: false

  signal featureSelectRequested( var featureId )

  function clearStackAndClose() {
    if ( browseDataLayout.depth > 1 )
      browseDataLayout.pop( null ) // pops everything besides an initialItem
    root.visible = false
  }

  function loadFeaturesFromLayerIndex( index ) {
    let modelIndex = __browseDataLayersModel.index( index, 0 )
    let layer = __browseDataLayersModel.data( modelIndex, BrowseDataLayersModel.VectorLayerRole )

    __featuresModel.reloadDataFromLayer( layer )
  }

  StackView {
    id: browseDataLayout
    initialItem: browseDataLayersPanel
    anchors.fill: parent
  }

  Component {
    id: browseDataLayersPanel

    BrowseDataLayersPanel {
      onBackButtonClicked: clearStackAndClose()
      onLayerClicked: {
        loadFeaturesFromLayerIndex( index )
        browseDataLayout.push( browseDataFeaturesPanel )
      }
    }
  }

  Component {
    id: browseDataFeaturesPanel

    BrowseDataFeaturesPanel {
      onBackButtonClicked: browseDataLayout.pop()
      onFeatureClicked: {
        clearStackAndClose()
        root.featureSelectRequested( featureId )
      }
    }
  }
}
