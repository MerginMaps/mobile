import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
  id: root
  visible: false

  signal featureSelectRequested( string featureName )

  function clearStackAndClose() {
    if ( browseDataLayout.depth > 1 )
      browseDataLayout.pop( null ) // pops everything besides an initialItem
    root.visible = false
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
        __featuresModel.reloadDataFromLayerName( layerName )
        browseDataLayout.push(browseDataFeaturesPanel, {selectedLayer: layerName})
      }
    }
  }

  Component {
    id: browseDataFeaturesPanel

    BrowseDataFeaturesPanel {
      onBackButtonClicked: browseDataLayout.pop()
      onFeatureClicked: {
        clearStackAndClose()
        root.featureSelectRequested( featureName )
      }
    }
  }
}
