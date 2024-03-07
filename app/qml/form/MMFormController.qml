/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls

import mm 1.0 as MM

// Wraps preview panel and feature form
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

  property alias formState: featureForm.state // add, edit or ReadOnly
  property alias panelState: statesManager.state

  property real previewHeight
  property real panelHeight

  property bool layerIsReadOnly: featureLayerPair?.layer?.readOnly ?? false

  signal closed()
  signal editGeometry( var pair )
  signal openLinkedFeature( var linkedFeature )
  signal createLinkedFeature( var targetLayer, var parentPair )
  signal stakeoutFeature( var feature )

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
          PropertyChanges { target: featureForm; visible: false }
          PropertyChanges { target: previewPanel; visible: true }
        },
        State {
          name: "form"
          PropertyChanges { target: drawer; height: root.height }
          PropertyChanges { target: drawer; interactive: false }
          PropertyChanges { target: featureForm; visible: true }
          PropertyChanges { target: previewPanel; visible: false }
          StateChangeScript {
            script: {
              featureForm.forceActiveFocus()
            }
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

    background: Rectangle { // rounded drawer
      color: __style.whiteColor
      radius: 20 * __dp

      Rectangle {
        width: parent.width / 10
        height: 4 * __dp

        anchors.top: parent.top
        anchors.topMargin: 8 * __dp
        anchors.horizontalCenter: parent.horizontalCenter

        radius: 20 * __dp

        color: __style.lightGreenColor
      }

      Rectangle {
        color: __style.whiteColor
        width: parent.width
        height: parent.height
        y: parent.height / 2
      }
    }

    width: parent.width

    modal: false
    dragMargin: 0 // prevents opening the drawer by dragging.

    edge: Qt.BottomEdge
    closePolicy: Popup.CloseOnEscape // prevents the drawer closing while moving canvas

    onClosed: {
      if ( statesManager.state !== "hidden" )
        statesManager.state = "closed"
    }

    MMPreviewDrawer {
      id: previewPanel

      layerIsReadOnly: root.layerIsReadOnly
      controller: MM.AttributePreviewController { project: root.project; featureLayerPair: root.featureLayerPair }

      height: root.previewHeight
      width: root.width

      onStakeoutClicked: function( feature ) {
        root.stakeoutFeature( feature )
      }

      onContentClicked: root.panelState = "form"

      onEditClicked: {
        root.panelState = "form"
        featureForm.state = "edit"
      }
    }

    MMFormPage {
      id: featureForm

      anchors.fill: parent

      project: root.project

      controller: MM.AttributeController {
        variablesManager: __variablesManager

        rememberAttributesController: MM.RememberAttributesController {
          rememberValuesAllowed: __appSettings.reuseLastEnteredValues
        }
        // NOTE: order matters, we want to init variables manager before
        // assingning FeatureLayerPair, as VariablesManager is required
        // for correct expression evaluation
        featureLayerPair: root.featureLayerPair
      }

      layerIsReadOnly: root.layerIsReadOnly

      onSaved: root.panelState = "closed"
      onCanceled: root.panelState = "closed"

      onEditGeometryRequested: function( pair ) {
        root.panelState = "hidden"
        root.editGeometry( pair )
      }

      onOpenLinkedFeature: function( linkedFeature ) {
        root.openLinkedFeature( linkedFeature )
      }

      onCreateLinkedFeature: function( parentController, relation ) {
        root.controllerToApply = parentController
        root.relationToApply = relation
        root.createLinkedFeature( relation.referencingLayer, root.featureLayerPair )
      }

      Connections {
        target: root
        function onFormStateChanged() {
          featureForm.state = root.formState
        }
      }

      Component.onCompleted: {
        if ( root.parentController && root.linkedRelation ) {
          featureForm.controller.parentController = root.parentController
          featureForm.controller.linkedRelation = root.linkedRelation
        }
      }
    }
  }
}
