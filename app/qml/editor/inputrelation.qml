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

import QgsQuick 0.1 as QgsQuick
import lc 1.0
import ".."

Item {
  id: root

  property int linkedFeaturesCount: rmodel.rowCount()
  property real expandedHeight: customStyle.fields.height * 3 // three rows

  signal valueChanged( var value, bool isNull )
  signal featureLayerPairChanged()

  signal openLinkedFeature( var linkedFeature )
  signal createLinkedFeature( var parentFeature, var relation )

  onFeatureLayerPairChanged: {
    // new feature layer pair, revert state and update delegate model
    if (rmodel.isTextType) {
      textModeContainer.state = "initial"
    }
    delegateModel.update()
  }

  RelationFeaturesModel {
    id: rmodel

    relation: associatedRelation
    parentFeatureLayerPair: featurePair
    homePath: activeProject.homePath

    onModelReset: root.linkedFeaturesCount = rowCount()

    onFeaturesCountChanged: delegateModel.update()
  }

  DelegateModel {
    id: delegateModel
    /*
     * Inspired by https://martin.rpdev.net/2019/01/15/using-delegatemodel-in-qml-for-sorting-and-filtering.html
     */

    property var filterAcceptsItem: function( item ) {
      if ( textModeContainer.state === "initial" ) {
        return false
      }
      return true
    }

    function update() {
      // reset all items to default (invisible) group
      if (items.count > 0) {
        items.setGroups(0, items.count, "items");
      }

      for ( var i = 0; i < items.count; ++i ) {
        var item = items.get( i );
        if ( filterAcceptsItem( item.model ) ) {
          item.inVisible = true;
        }
      }
    }

    model: rmodel
    delegate: textDelegate

    groups: DelegateModelGroup {
      id: visibleItems

      name: "visible"
      includeByDefault: false
    }

    filterOnGroup: "visible"
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

      states: [
        State {
          name: "initial"
          PropertyChanges {
            target: noOfFeaturesText
            visible: true
          }
          PropertyChanges {
            target: textModeContainer
            height: customStyle.fields.height
          }
        },
        State {
          name: "expanded"
          PropertyChanges {
            target: textModeContainer
            height: root.expandedHeight //customStyle.fields.height * 3 // three rows
          }
          PropertyChanges {
            target: noOfFeaturesText
            visible: false
          }
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

      transitions: [
        Transition {
          from: "initial"
          to: "expanded"
          animations: NumberAnimation { property: "height"; duration: 100 }
        }
      ]

      onStateChanged: delegateModel.update()

      visible: rmodel.isTextType
      height: customStyle.fields.height
      width: parent.width
      state: "initial"

      border.color: customStyle.fields.normalColor
      border.width: 1 * QgsQuick.Utils.dp
      color: customStyle.fields.backgroundColor
      radius: customStyle.fields.cornerRadius

      Text {
        id: noOfFeaturesText

        anchors.fill: parent
        font.pointSize: customStyle.fields.fontPointSize
        color: customStyle.fields.fontColor
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        text: qsTr( "%n linked feature(s)", "Shows how many features are linked via relations", root.linkedFeaturesCount )
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          if ( textModeContainer.state === "initial" )
            textModeContainer.state = "expanded"
          else if ( textModeContainer.state === "expanded" )
            textModeContainer.state = "page"
        }
      }

      Flow {
        id: flowItemView

        anchors.fill: parent
        anchors.margins: customStyle.fields.sideMargin

        spacing: customStyle.group.spacing

        Repeater { model: delegateModel; }
      }
    }

    // Photo Mode Widget
    Rectangle {
      id: photoModeContainer

      visible: !rmodel.isTextType
      height: expandedHeight
      width: parent.width

      border.color: customStyle.fields.normalColor
      border.width: 1 * QgsQuick.Utils.dp
      color: customStyle.fields.backgroundColor
      radius: customStyle.fields.cornerRadius

      ListView {
        height: expandedHeight
        width: parent.width
        anchors.margins: customStyle.fields.sideMargin
        anchors.fill: parent
        spacing: customStyle.group.spacing
        orientation: ListView.Horizontal
        clip: true

        model: rmodel
        delegate: photoDelegate
      }
    }
  }

  Component {
    id: textDelegate

    Item {
      id: textDelegateContainer

      property real maximumWidth: flowItemView.width
      property bool isVisible: {
        // figure out which row am I from Y
        if ( y === 0 ) return true
        else if ( y < 3 * height ) return true
        else return false
      }

      height: customStyle.relationComponent.textDelegateHeight
      width: childrenRect.width > maximumWidth ? maximumWidth : childrenRect.width

      visible: isVisible

      Rectangle {
        id: textDelegateContent

        property real requestedWidth: txt.paintedWidth + 10 * QgsQuick.Utils.dp

        height: parent.height
        width: requestedWidth > parent.maximumWidth ? parent.maximumWidth : requestedWidth

        color: customStyle.fields.backgroundColorDark
        radius: customStyle.fields.cornerRadius
        border.color: customStyle.fields.backgroundColorDarker
        border.width: 2 * QgsQuick.Utils.dp

        Text {
          id: txt

          text: model.FeatureTitle

          width: parent.width
          height: parent.height
          horizontalAlignment: Qt.AlignHCenter
          verticalAlignment: Qt.AlignVCenter

          font.pointSize: customStyle.fields.fontPointSize
          color: customStyle.fields.fontColor
          clip: true
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: root.openLinkedFeature( model.FeaturePair )
      }
    }
  }

  Component {
    id: photoDelegate

    Item {
      height: expandedHeight
      width: height

      Image {
        id: image

        anchors.fill: parent
        sourceSize.width: image.width
        sourceSize.height: image.height
        source: {
          let absolutePath = model.PhotoPath

          if (image.status === Image.Error) {
            customStyle.icons.brokenImage
          }
          else if (absolutePath !== '' && __inputUtils.fileExists(absolutePath)) {
            "file://" + absolutePath
          }
          else if (absolutePath === '' || absolutePath === undefined) {
            customStyle.icons.notAvailable
          } else {
            customStyle.icons.brokenImage
          }
        }
        mipmap: true
        fillMode: Image.PreserveAspectFit
      }

      MouseArea {
        anchors.fill: parent
        onClicked: root.openLinkedFeature( model.FeaturePair )
        // TODO onPressAndHold
      }

    }
  }

  Component {
    id: relationsPageComponent

    BrowseDataFeaturesPanel {
      id: relationsPage

      pageTitle: qsTr( "Linked features" )
      allowSearch: false //TODO search
      layerHasGeometry: false
      toolbarVisible: !root.parent.readOnly
      focus: true

      onBackButtonClicked: {
        root.parent.formView.pop()
        textModeContainer.state = "expanded"
      }

      onAddFeatureClicked: root.createLinkedFeature( root.parent.featurePair, root.parent.associatedRelation )
      onFeatureClicked: {
        let clickedFeature = featuresModel.attributeFromValue( FeaturesListModel.FeatureId, featureIds, FeaturesListModel.FeaturePair)
        root.openLinkedFeature( clickedFeature )
      }

      Keys.onReleased: {
        if ( event.key === Qt.Key_Back || event.key === Qt.Key_Escape ) {
          event.accepted = true
          root.parent.formView.pop()
          textModeContainer.state = "expanded"
        }
      }
    }
  }
}
