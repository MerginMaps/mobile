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
import QtQml.Models 2.14
import QtQuick.Layouts 1.14

import lc 1.0
import ".."

Item {
  id: root

  property real widgetHeight: customStyle.fields.height * 3 // three rows

  signal featureLayerPairChanged()

  signal openLinkedFeature( var linkedFeature )
  signal createLinkedFeature( var parentFeature, var relation )

  RelationFeaturesModel {
    id: rmodel

    relation: associatedRelation
    parentFeatureLayerPair: featurePair
    homePath: activeProject.homePath
  }

  anchors {
    left: parent.left
    right: parent.right
  }

  height: content.height

  Item {
    id: content

    height: rmodel.isTextType ? textModeContainer.height : photoModeContainer.height
    anchors {
      left: parent.left
      right: parent.right
    }

    // Text Mode Widget
    Rectangle {
      id: textModeContainer

      property real fullLineWidth: flowItemView.width // full line width - first lines
      property real lastLineShorterWidth: flowItemView.width - addChildButton.width - ( showMoreButton.visible ? showMoreButton.width : 0 )
      property int invisibleItemsCounter: 0

      states: [
        State {
          name: "initial"
        },
        State {
          name: "page"
          StateChangeScript {
            script: {
              let page = root.parent.formView.push( relationsPageComponent, { featuresModel: rmodel } )
              page.forceActiveFocus()
            }
          }
        }
      ]

      visible: rmodel.isTextType
      height: root.widgetHeight
      width: parent.width
      state: "initial"

      Layout.maximumHeight: root.widgetHeight
      Layout.minimumHeight: customStyle.fields.height

      border.color: customStyle.fields.normalColor
      border.width: 1 * __dp
      color: customStyle.fields.backgroundColor
      radius: customStyle.fields.cornerRadius

      MouseArea {
        anchors.fill: parent
        onClicked: {
          if ( textModeContainer.state === "initial" )
            textModeContainer.state = "page"
        }
      }

      Flow {
        id: flowItemView

        anchors.fill: parent
        anchors.margins: customStyle.fields.sideMargin

        spacing: customStyle.relationComponent.flowSpacing

        Repeater {
          model: rmodel
          delegate: RelationTextDelegate {
            firstLinesMaxWidth: flowItemView.width
            lastLineMaxWidth: flowItemView.width / 2

            onClicked: root.openLinkedFeature( feature )
            onSetInvisible: textModeContainer.invisibleItemsCounter++
          }
        }

        RelationTextDelegate {
          id: showMoreButton

          isVisible: textModeContainer.invisibleItemsCounter > 0
          text: qsTr( "%1 more" ).arg( textModeContainer.invisibleItemsCounter )

          firstLinesMaxWidth: textModeContainer.fullLineWidth
          lastLineMaxWidth: firstLinesMaxWidth

          backgroundContent.color: customStyle.relationComponent.tagBackgroundColorButton
          backgroundContent.border.color: customStyle.relationComponent.tagBorderColorButton
          textContent.color: customStyle.relationComponent.tagTextColor

          onClicked: textModeContainer.state = "page"
        }

        RelationTextDelegate {
          id: addChildButton

          text: "+ " + qsTr( "Add" )
          isVisible: !root.parent.readOnly

          backgroundContent.color: customStyle.relationComponent.tagBackgroundColorButtonAlt
          backgroundContent.border.color: customStyle.relationComponent.tagBorderColorButton
          textContent.color: customStyle.relationComponent.tagTextColorButton

          firstLinesMaxWidth: textModeContainer.fullLineWidth
          lastLineMaxWidth: firstLinesMaxWidth

          onClicked: root.createLinkedFeature( root.parent.featurePair, root.parent.associatedRelation )
        }
      }
    }

    // Photo Mode Widget
    Rectangle {
      id: photoModeContainer

      visible: !rmodel.isTextType
      height: widgetHeight
      width: parent.width

      border.color: customStyle.fields.normalColor
      border.width: 1 * __dp
      color: customStyle.fields.backgroundColor
      radius: customStyle.fields.cornerRadius

      ListView {
        height: widgetHeight
        width: parent.width
        anchors.margins: customStyle.fields.sideMargin
        anchors.fill: parent
        spacing: customStyle.group.spacing
        orientation: ListView.Horizontal
        clip: true

        model: rmodel
        delegate: RelationPhotoDelegate {
          onClicked: root.openLinkedFeature( feature )
        }

        footer: RelationPhotoFooterDelegate {
          isReadOnly: root.parent.readOnly
          onClicked: root.createLinkedFeature( root.parent.featurePair, root.parent.associatedRelation )
        }
      }
    }
  }

  Component {
    id: relationsPageComponent

    FeaturesListPage {
      id: relationsPage

      pageTitle: qsTr( "Linked features" )
      allowSearch: false //TODO search
      toolbarButtons: ["add"]
      toolbarVisible: !root.parent.readOnly
      focus: true

      onBackButtonClicked: {
        root.parent.formView.pop()
        textModeContainer.state = "initial"
      }

      onAddFeatureClicked: root.createLinkedFeature( root.parent.featurePair, root.parent.associatedRelation )
      onSelectionFinished: {
        let clickedFeature = featuresModel.convertRoleValue( FeaturesModel.FeatureId, featureIds, FeaturesModel.FeaturePair )
        root.openLinkedFeature( clickedFeature )
      }

      Keys.onReleased: {
        if ( event.key === Qt.Key_Back || event.key === Qt.Key_Escape ) {
          event.accepted = true
          root.parent.formView.pop()
          textModeContainer.state = "initial"
        }
      }
    }
  }
}
