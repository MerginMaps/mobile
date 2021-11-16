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
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "../"

Item {
  id: root
  property real widgetHeight: customStyle.fields.height * 3 // three rows

  height: content.height

  signal createLinkedFeature(var parentFeature, var relation)
  signal featureLayerPairChanged
  signal openLinkedFeature(var linkedFeature)

  RelationFeaturesModel {
    id: rmodel
    homePath: activeProject.homePath
    parentFeatureLayerPair: featurePair
    relation: associatedRelation
  }
  anchors {
    left: parent.left
    right: parent.right
  }
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
      property int invisibleItemsCounter: 0
      property real lastLineShorterWidth: flowItemView.width - addChildButton.width - (showMoreButton.visible ? showMoreButton.width : 0)

      Layout.maximumHeight: root.widgetHeight
      Layout.minimumHeight: customStyle.fields.height
      border.color: customStyle.fields.normalColor
      border.width: 1 * QgsQuick.Utils.dp
      color: customStyle.fields.backgroundColor
      height: root.widgetHeight
      radius: customStyle.fields.cornerRadius
      state: "initial"
      visible: rmodel.isTextType
      width: parent.width

      MouseArea {
        anchors.fill: parent

        onClicked: {
          if (textModeContainer.state === "initial")
            textModeContainer.state = "page";
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

            onClicked: root.openLinkedFeature(feature)
            onSetInvisible: textModeContainer.invisibleItemsCounter++
          }
        }
        RelationTextDelegate {
          id: showMoreButton
          backgroundContent.border.color: customStyle.relationComponent.tagBorderColorButton
          backgroundContent.color: customStyle.relationComponent.tagBackgroundColorButton
          firstLinesMaxWidth: textModeContainer.fullLineWidth
          isVisible: textModeContainer.invisibleItemsCounter > 0
          lastLineMaxWidth: firstLinesMaxWidth
          text: qsTr("%1 more").arg(textModeContainer.invisibleItemsCounter)
          textContent.color: customStyle.relationComponent.tagTextColor

          onClicked: textModeContainer.state = "page"
        }
        RelationTextDelegate {
          id: addChildButton
          backgroundContent.border.color: customStyle.relationComponent.tagBorderColorButton
          backgroundContent.color: customStyle.relationComponent.tagBackgroundColorButtonAlt
          firstLinesMaxWidth: textModeContainer.fullLineWidth
          isVisible: !root.parent.readOnly
          lastLineMaxWidth: firstLinesMaxWidth
          text: "+ " + qsTr("Add")
          textContent.color: customStyle.relationComponent.tagTextColorButton

          onClicked: root.createLinkedFeature(root.parent.featurePair, root.parent.associatedRelation)
        }
      }

      states: [
        State {
          name: "initial"
        },
        State {
          name: "page"

          StateChangeScript {
            script: {
              let page = root.parent.formView.push(relationsPageComponent, {
                  "featuresModel": rmodel
                });
              page.forceActiveFocus();
            }
          }
        }
      ]
    }

    // Photo Mode Widget
    Rectangle {
      id: photoModeContainer
      border.color: customStyle.fields.normalColor
      border.width: 1 * QgsQuick.Utils.dp
      color: customStyle.fields.backgroundColor
      height: widgetHeight
      radius: customStyle.fields.cornerRadius
      visible: !rmodel.isTextType
      width: parent.width

      ListView {
        anchors.fill: parent
        anchors.margins: customStyle.fields.sideMargin
        clip: true
        height: widgetHeight
        model: rmodel
        orientation: ListView.Horizontal
        spacing: customStyle.group.spacing
        width: parent.width

        delegate: RelationPhotoDelegate {
          onClicked: root.openLinkedFeature(feature)
        }
        footer: RelationPhotoFooterDelegate {
          isReadOnly: root.parent.readOnly

          onClicked: root.createLinkedFeature(root.parent.featurePair, root.parent.associatedRelation)
        }
      }
    }
  }
  Component {
    id: relationsPageComponent
    FeaturesListPage {
      id: relationsPage
      allowSearch: false //TODO search
      focus: true
      pageTitle: qsTr("Linked features")
      toolbarButtons: ["add"]
      toolbarVisible: !root.parent.readOnly

      Keys.onReleased: {
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
          event.accepted = true;
          root.parent.formView.pop();
          textModeContainer.state = "initial";
        }
      }
      onAddFeatureClicked: root.createLinkedFeature(root.parent.featurePair, root.parent.associatedRelation)
      onBackButtonClicked: {
        root.parent.formView.pop();
        textModeContainer.state = "initial";
      }
      onSelectionFinished: {
        let clickedFeature = featuresModel.convertRoleValue(FeaturesModel.FeatureId, featureIds, FeaturesModel.FeaturePair);
        root.openLinkedFeature(clickedFeature);
      }
    }
  }
}
