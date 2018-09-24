import QtQuick 2.7
import QtQuick.Controls 2.2
import QgsQuick 0.1 as QgsQuick
import "."  // import YdnpaStyle singleton

Drawer {

    property var mapSettings
    property var project

    property alias state: featureForm.state
    property alias feature: attributeModel.featureLayerPair
    property alias currentAttributeModel: attributeModel

    id: featurePanel
    visible: false
    modal: true
    interactive: true
    dragMargin: 0 // prevents opening the drawer by dragging.

    background: Rectangle {
        color: InputStyle.clrPanelBackground
        opacity: InputStyle.panelOpacity
    }

    function show_panel(feature, state) {
      featurePanel.feature = feature
      featurePanel.state = state
      featurePanel.visible = true
    }

//    function show_panel(layer, feature, state) {
//        if (QgsQuick.Utils.hasValidGeometry(layer, feature)) {
//            // layer needs to be set before the feature otherwise the panel ends up empty on layer change
//            // TODO: fix the panel so that the order does not matter
//            featurePanel.layer = layer
//            featurePanel.feature = feature
//            featurePanel.state = state

//            // visible needs to be after setting correct layer&feature,
//            // so currentFeatureModel is already up to date (required for feature highlight)
//            featurePanel.visible = true
//        } else {
//            QgsQuick.Utils.logMessage("The feature " + layer.name + " has a wrong geometry." , "Input")
//        }
//    }

    QgsQuick.FeatureForm {
      id: featureForm

      // using anchors here is not working well as
      width: featurePanel.width
      height: featurePanel.height

      model: QgsQuick.AttributeFormModel {
        attributeModel: QgsQuick.AttributeModel {
            id: attributeModel
        }
      }

      project: featurePanel.project

      toolbarVisible: true

      onSaved: {
        featurePanel.visible = false
      }
      onCanceled: featurePanel.visible = false
    }

}
