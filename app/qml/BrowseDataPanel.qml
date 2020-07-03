import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
  id: root
  visible: false

  signal featureSelectRequested( var featureId )

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
        __featuresModel.reloadDataFromLayer( layer )
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
