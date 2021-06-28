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

  property var featureLayerPair
  property var parentFeatureLayerPair
  property var project
  property string startingState // preview or form
  property string formState // add, edit or ReadOnly

  property real previewHeight
  property real panelHeight

  property bool isReadOnly: featureLayerPair ? featureLayerPair.layer.readOnly : false

  signal createFeature( var layer )

  Drawer {
    id: drawer

    Item {
      id: statesManager

      state: root.startingState
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
        }
      ]

      onStateChanged: {
        if (state === "preview")
          drawer.open()
      }
    }

    Behavior on height {
      PropertyAnimation { properties: "height"; easing.type: Easing.InOutQuad }
    }

    background: Rectangle {
      color: InputStyle.clrPanelMain
    }

    width: parent.width
    z: 0
    modal: false
    dragMargin: 0 // prevents opening the drawer by dragging.
    edge: Qt.BottomEdge
    closePolicy: Popup.CloseOnEscape // prevents the drawer closing while moving canvas

    PreviewPanel {
      id: previewPanel

      isReadOnly: root.isReadOnly
      controller: AttributePreviewController { project: root.project; featureLayerPair: root.featureLayerPair }

      height: root.previewHeight
      width: root.width

      onContentClicked: {
        statesManager.state = "form"
      }

      onEditClicked: {
        statesManager.state = "form"
        formContainer.formState = "Edit"
      }
    }

    FeatureFormPage {
      id: formContainer

      project: root.project
      featureLayerPair: root.featureLayerPair
      parentFeatureLayerPair: root.parentFeatureLayerPair
      formState: root.formState

      anchors.fill: parent
    }
  }
}
