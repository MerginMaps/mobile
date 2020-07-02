import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
  id: root
  visible: false

  StackView {
    id: browseDataLayout
    initialItem: browseDataLayersPanel
    anchors.fill: parent
  }

  Component {
    id: browseDataLayersPanel

    BrowseDataLayersPanel {
      onBackButtonClicked: root.visible = false
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
    }
  }
}
