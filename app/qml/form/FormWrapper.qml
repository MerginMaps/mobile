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

  // for child features in relation:
  property var linkedRelation
  property var parentController

  property alias formState: formContainer.formState // add, edit or ReadOnly
  property alias panelState: statesManager.state

  property real previewHeight
  property real panelHeight

  property bool isReadOnly: featureLayerPair ? featureLayerPair.layer.readOnly : false

  signal closed()
  signal closeDrawer()
  signal openLinkedFeature( var linkedFeature )
  signal createLinkedFeature( var parentController, var relation )

  function isNewFeature() {
    return formContainer.isNewFeature()
  }

  onCloseDrawer: drawer.close()

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
        }
      ]

      onStateChanged: {
        switch( state ) {
          case "form":
          case "preview":
            drawer.open();
            break;
          case "closed":
            root.closed()
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

    onClosed: statesManager.state = "closed"

    PreviewPanel {
      id: previewPanel

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
      onEditGeometryClicked: console.log( "NOT IMPLEMENTED" )
      onOpenLinkedFeature: root.openLinkedFeature( linkedFeature )
      onCreateLinkedFeature: root.createLinkedFeature( parentController, relation )
    }
  }
}
