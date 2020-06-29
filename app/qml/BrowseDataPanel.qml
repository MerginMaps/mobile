import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
  id: browseDataView
  visible: false

  StackView {
    id: browseDataStackView
    initialItem: layersListPanel
    anchors.fill: parent
  }

  Component {
    id: layersListPanel

    LayersListPanel {
      onBackButtonClicked: browseDataView.visible = false
      onLayerClicked: {
        browseDataStackView.push(layerAttributesPanel, {selectedLayer: layerName})
      }
    }
  }

  Component {
    id: layerAttributesPanel

    LayerAttributesPanel {
      onBackButtonClicked: browseDataStackView.pop()
    }
  }
}
