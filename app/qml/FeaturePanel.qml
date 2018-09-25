import QtQuick 2.7
import QtQuick.Controls 2.2
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

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
