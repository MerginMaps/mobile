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
import ".."

Item {
  id: root

  property var project
  property var featureLayerPair

  // child features in relations need to have these set in order to prefill their foreign keys
  property var linkedRelation
  property var parentController

  // intermediate store for properties that will be assigned to linked feature when it is created.
  // these properties are set only while future linked feature is beign digitized
  property var relationToApply
  property var controllerToApply

  property alias formState: formContainer.formState // add, edit or ReadOnly
  property alias panelState: statesManager.state

  property real previewHeight
  property real panelHeight

  property bool isReadOnly: featureLayerPair ? featureLayerPair.layer.readOnly : false

  signal closed()
  signal editGeometry( var pair )
  signal openLinkedFeature( var linkedFeature )
  signal createLinkedFeature( var targetLayer, var parentPair )
  signal stakeoutFeature( var feature )

  function updateFeatureGeometry() {
    formContainer.updateFeatureGeometry()
  }

  function openDrawer() {
    root.panelState = "form"
  }

  function closeDrawer() {
    drawer.close()
  }

  Drawer {
    id: drawer

    StateGroup {
      id: statesManager

      state: root.panelState
      states: [
        State {
          name: "preview"
          PropertyChanges { target: drawer; height: root.previewHeight }
          PropertyChanges { target: drawer; interactive: true }
          PropertyChanges { target: formContainer; visible: false }
          PropertyChanges { target: previewPanel; visible: true }
        },
        State {
          name: "form"
          PropertyChanges { target: drawer; height: root.height }
          PropertyChanges { target: drawer; interactive: false }
          PropertyChanges { target: formContainer; visible: true }
          PropertyChanges { target: previewPanel; visible: false }
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

      onStateChanged: {
        switch( state ) {
          case "form":
          case "preview":
            drawer.open();
            break;
          case "closed":
            root.closed();
            break;
        }
      }
    }

    Behavior on height {
      PropertyAnimation { properties: "height"; easing.type: Easing.InOutQuad }
    }

    width: parent.width
    z: 0
    modal: false
    dragMargin: 0 // prevents opening the drawer by dragging.
    edge: Qt.BottomEdge
    closePolicy: Popup.CloseOnEscape // prevents the drawer closing while moving canvas

    onClosed: {
      if ( statesManager.state !== "hidden" )
        statesManager.state = "closed"
    }

    PreviewPanel {
      id: previewPanel

      onStakeoutFeature: root.stakeoutFeature( feature )

      isReadOnly: root.isReadOnly
      controller: AttributePreviewController { project: root.project; featureLayerPair: root.featureLayerPair }

      height: root.previewHeight
      width: root.width

      onContentClicked: root.panelState = "form"
      onEditClicked: {
        root.panelState = "form"
        formContainer.formState = "edit"
      }
    }

    FeatureFormPage {
      id: formContainer

      anchors.fill: parent

      project: root.project
      featureLayerPair: root.featureLayerPair

      linkedRelation: root.linkedRelation
      parentController: root.parentController

      formState: root.formState

      onClose: root.panelState = "closed"
      onEditGeometryClicked: {
        root.panelState = "hidden"
        root.editGeometry( root.featureLayerPair )
      }
      onOpenLinkedFeature: root.openLinkedFeature( linkedFeature )
      onCreateLinkedFeature: {
        root.controllerToApply = parentController
        root.relationToApply = relation
        root.createLinkedFeature( relation.referencingLayer, root.featureLayerPair )
      }
    }
  }
}
