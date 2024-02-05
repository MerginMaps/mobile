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
import QtQuick.Controls.Basic

import lc 1.0

import "../../components"
import "../../inputs"

/*
 * Relation editor (text mode ~~ bubbles/word cloud) for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 */

MMBaseInput {
  id: root

  property var _fieldAssociatedRelation: parent.fieldAssociatedRelation
  property var _fieldFeatureLayerPair: parent.fieldFeatureLayerPair
  property var _fieldActiveProject: parent.fieldActiveProject

  property string _fieldTitle: parent.fieldTitle
  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle

  property ListModel featuresModel // <---- what to do here?

  signal openLinkedFeature( var linkedFeature )
  signal createLinkedFeature( var parentFeature, var relation )

  contentItemHeight: privates.itemHeight * privates.rows + 2 * flow.spacing + 20 * __dp

  Component.onCompleted: root.recalculate()
  onWidthChanged: root.recalculate()

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  content: Rectangle {
    width: root.width - 2 * root.spacing
    height: root.contentItemHeight
    color: __style.whiteColor

    Flow {
      id: flow

      anchors.fill: parent
      anchors.margins: 10 * __dp
      spacing: 8 * __dp
      clip: true

      Rectangle {
        width: 40 * __dp
        height: privates.itemHeight
        radius: 8 * __dp
        color: __style.lightGreenColor

        MMIcon {
          anchors.centerIn: parent
          source: __style.plusIcon
          useCustomSize: true
          width: 16 * __dp
          height: width
        }

        MouseArea {
          anchors.fill: parent
          onClicked: root.createLinkedFeature( root._fieldFeatureLayerPair, root._fieldAssociatedRelation )
        }
      }

      Repeater {
        id: repeater

        property var invisibleIds: 0

        model: RelationFeaturesModel {
          id: rmodel

          relation: root._fieldAssociatedRelation
          parentFeatureLayerPair: root._fieldFeatureLayerPair
          homePath: root._fieldActiveProject.homePath

          onModelReset: {
            // Repeater does not necesarry clear delegates immediately if they are invisible,
            // we need to do hard reload in this case so that recalculateVisibleItems() is triggered

            root.recalculate()
          }
        }

        delegate: Rectangle {
          width: text.contentWidth + 24 * __dp
          height: privates.itemHeight
          radius: 8 * __dp
          color: __style.mediumGreenColor
          visible: (y < 2 * privates.itemHeight || (y < 3 * privates.itemHeight && x + width < flow.width - footer.width - flow.spacing))

          Text {
            id: text

            anchors.centerIn: parent
            font: __style.t4
            text: model.FeatureTitle
            color: __style.forestColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
          }

          MouseArea {
            anchors.fill: parent
            onClicked: root.openLinkedFeature( model.FeaturePair )
          }

          onVisibleChanged: {
            if(!visible) {
              repeater.invisibleIds++
            }
          }
        }
      }

      Rectangle {
        id: footer

        width: 100 * __dp
        height: privates.itemHeight
        visible: repeater.invisibleIds > 0
        radius: 8 * __dp
        color: __style.lightGreenColor

        Text {
          id: moreText

          anchors.centerIn: parent
          font: __style.t4
          color: __style.forestColor
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          text: "+" + repeater.invisibleIds + qsTr(" more")
        }

        MouseArea {
          anchors.fill: parent
          onClicked: {
            if ( !root.enabled )
              return
            listLoader.active = true
            listLoader.focus = true
          }
        }
      }
    }
  }

  function recalculate() {
    repeater.invisibleIds = 0
    repeater.model = null
    repeater.model = rmodel
  }

  Loader {
    id: listLoader

    asynchronous: true
    active: false
    sourceComponent: listComponent
  }

  Component {
    id: listComponent

    MMFeaturesListDrawer {
      focus: true
      model: rmodel
      title: root._fieldTitle
      withSearch: false

      Component.onCompleted: open()
      onClosed: listLoader.active = false
      onFeatureClicked: function(selectedFeatures) {
        root.openLinkedFeature( selectedFeatures )
      }
      onCreateLinkedFeature: root.createLinkedFeature( root._fieldFeatureLayerPair, root._fieldAssociatedRelation )
    }
  }

  QtObject {
    id: privates

    readonly property real itemHeight: 36 * __dp
    readonly property int rows: 3
  }
}
