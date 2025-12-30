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
import MMInput

import "../../components" as MMComponents
import "../../components/private" as MMPrivateComponents
import "../components" as MMFormComponents

/*
 * Relation editor (text mode ~~ bubbles/word cloud) for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 */

MMPrivateComponents.MMBaseInput {
  id: root

  property var _fieldAssociatedRelation: parent.fieldAssociatedRelation
  property var _fieldFeatureLayerPair: parent.fieldFeatureLayerPair
  property var _fieldActiveProject: parent.fieldActiveProject

  property string _fieldTitle: parent.fieldTitle
  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle

  signal openLinkedFeature( var linkedFeature )
  signal createLinkedFeature( var parentFeature, var relation )

  Component.onCompleted: root.recalculateVisibleItems()
  onWidthChanged: root.recalculateVisibleItems()

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  inputContent: Rectangle {
    width: parent.width
    height: privates.itemHeight * privates.rows + 2 * flow.spacing + 2 * __style.margin12

    radius: __style.radius12
    color: __style.polarColor

    MouseArea {
      anchors.fill: parent
      onClicked: function( mouse ) {
        mouse.accepted = true
        listLoader.active = true
        listLoader.focus = true
      }
      enabled: repeater.count > 0 || newRelation.visible
    }

    Flow {
      id: flow

      anchors {
        fill: parent
        topMargin: __style.margin12
        bottomMargin: __style.margin12
        leftMargin: __style.margin20
        rightMargin: __style.margin20
      }

      spacing: __style.margin8
      clip: true

      Rectangle {
        id: newRelation

        width: 40 * __dp
        height: privates.itemHeight
        radius: 8 * __dp
        color: __style.lightGreenColor
        visible: ActiveProject.projectRole !== "reader"

        MMComponents.MMIcon {
          anchors.centerIn: parent
          source: __style.plusIcon
          size: __style.icon16
          height: width
        }

        MMComponents.MMSingleClickMouseArea{
          anchors.fill: parent
          onSingleClicked: {
            root.forceActiveFocus() // clear focus from all elements to prevent freezing #3483
            root.createLinkedFeature( root._fieldFeatureLayerPair, root._fieldAssociatedRelation )
          }
        }
      }

      Repeater {
        id: repeater

        property var invisibleIds: 0

        model: MM.RelationFeaturesModel {
          id: rmodel

          relation: root._fieldAssociatedRelation
          parentFeatureLayerPair: root._fieldFeatureLayerPair
          homePath: root._fieldActiveProject.homePath

          onModelReset: {
            // Repeater does not necesarry clear delegates immediately if they are invisible,
            // we need to do hard reload in this case so that recalculateVisibleItems() is triggered

            repeater.model = null
            repeater.model = rmodel

            root.recalculateVisibleItems()
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

          MMComponents.MMSingleClickMouseArea {
            anchors.fill: parent
            onSingleClicked: root.openLinkedFeature( model.FeaturePair )
          }

          onVisibleChanged: root.recalculateVisibleItems()
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
            listLoader.active = true
            listLoader.focus = true
          }
        }
      }
    }
  }

  function recalculateVisibleItems() {
    let invisibles_count = 0

    for ( let i = 0; i < repeater.count; i++ ) {
      let delegate_i = repeater.itemAt( i )
      if ( delegate_i && !delegate_i.visible ) {
        invisibles_count++
      }
    }

    repeater.invisibleIds = invisibles_count
  }

  Loader {
    id: listLoader

    asynchronous: true
    active: false
    sourceComponent: listComponent
  }

  Component {
    id: listComponent

    MMFormComponents.MMFeaturesListPageDrawer {

      pageHeader.title: root._fieldTitle

      list.model: rmodel
      button.text: qsTr( "Add feature" )

      onClosed: listLoader.active = false
      onFeatureClicked: ( featurePair ) => root.openLinkedFeature( featurePair )
      onSearchTextChanged: ( searchText ) => rmodel.searchExpression = searchText
      onButtonClicked: root.createLinkedFeature( root._fieldFeatureLayerPair, root._fieldAssociatedRelation )

      Component.onCompleted: open()
    }
  }

  QtObject {
    id: privates

    readonly property real itemHeight: 36 * __dp
    readonly property int rows: 3
  }
}
