/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.14
import QtQuick.Controls 2.14
import lc 1.0
import "../"

Item {
  id: root
  property var controllerToApply
  property var featureLayerPair
  property alias formState: formContainer.formState // add, edit or ReadOnly
  property bool isReadOnly: featureLayerPair ? featureLayerPair.layer.readOnly : false

  // child features in relations need to have these set in order to prefill their foreign keys
  property var linkedRelation
  property real panelHeight
  property alias panelState: statesManager.state
  property var parentController
  property real previewHeight
  property var project

  // intermediate store for properties that will be assigned to linked feature when it is created.
  // these properties are set only while future linked feature is beign digitized
  property var relationToApply

  function closeDrawer() {
    drawer.close();
  }
  signal closed
  signal createLinkedFeature(var targetLayer, var parentPair)
  signal editGeometry(var pair)
  function openDrawer() {
    root.panelState = "form";
  }
  signal openLinkedFeature(var linkedFeature)
  function updateFeatureGeometry() {
    formContainer.updateFeatureGeometry();
  }

  Drawer {
    id: drawer
    closePolicy: Popup.CloseOnEscape // prevents the drawer closing while moving canvas
    dragMargin: 0 // prevents opening the drawer by dragging.
    edge: Qt.BottomEdge
    modal: false
    width: parent.width
    z: 0

    onClosed: {
      if (statesManager.state !== "hidden")
        statesManager.state = "closed";
    }

    StateGroup {
      id: statesManager
      state: root.panelState

      onStateChanged: {
        switch (state) {
        case "form":
        case "preview":
          drawer.open();
          break;
        case "closed":
          root.closed();
          break;
        }
      }

      states: [
        State {
          name: "preview"

          PropertyChanges {
            height: root.previewHeight
            target: drawer
          }
          PropertyChanges {
            interactive: true
            target: drawer
          }
          PropertyChanges {
            target: formContainer
            visible: false
          }
          PropertyChanges {
            target: previewPanel
            visible: true
          }
        },
        State {
          name: "form"

          PropertyChanges {
            height: root.height
            target: drawer
          }
          PropertyChanges {
            interactive: false
            target: drawer
          }
          PropertyChanges {
            target: formContainer
            visible: true
          }
          PropertyChanges {
            target: previewPanel
            visible: false
          }
        },
        State {
          name: "closed"
        },
        State {
          // state used to hide form when using map (editing geometry / adding linked features).
          // form is still instantiated and will be visible when map editing is finished
          name: "hidden"
        }
      ]
    }
    PreviewPanel {
      id: previewPanel
      height: root.previewHeight
      isReadOnly: root.isReadOnly
      width: root.width

      onContentClicked: root.panelState = "form"
      onEditClicked: {
        root.panelState = "form";
        formContainer.formState = "edit";
      }

      controller: AttributePreviewController {
        featureLayerPair: root.featureLayerPair
        project: root.project
      }
    }
    FeatureFormPage {
      id: formContainer
      anchors.fill: parent
      featureLayerPair: root.featureLayerPair
      formState: root.formState
      linkedRelation: root.linkedRelation
      parentController: root.parentController
      project: root.project

      onClose: root.panelState = "closed"
      onCreateLinkedFeature: {
        root.controllerToApply = parentController;
        root.relationToApply = relation;
        root.createLinkedFeature(relation.referencingLayer, root.featureLayerPair);
      }
      onEditGeometryClicked: {
        root.panelState = "hidden";
        root.editGeometry(root.featureLayerPair);
      }
      onOpenLinkedFeature: root.openLinkedFeature(linkedFeature)
    }

    Behavior on height  {
      PropertyAnimation {
        easing.type: Easing.InOutQuad
        properties: "height"
      }
    }
  }
}
