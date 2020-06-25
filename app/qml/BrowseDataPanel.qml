import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
  id: browseDataView
  visible: false
  anchors.fill:parent

  StackView {
    id: browseDataStackView
    initialItem: layersListPanel
    anchors.fill: parent
  }

  Component {
    id: layersListPanel

    LayersListPanel {
      onBackButtonTapped: browseDataView.visible = false
      onLayerClicked: {
        browseDataStackView.push(layerAttributesPanel, {selectedLayer: layerName})
      }
    }
  }

  Component {
    id: layerAttributesPanel

    LayerAttributesPanel {
      onBackButtonTapped: browseDataStackView.pop()
    }
  }
}
