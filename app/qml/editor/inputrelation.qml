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

Item {
  id: root

  property real iconSize: 15

  /**
   * Property mode sets how the relation widget will look like, there are two options:
   * - text: features from child layer are placed on grid (cell displayed as rectangle) with display string
   * - photo: list horizontal/verical of photos
   */
  property string mode: "text"
  property int linkedFeaturesCount: rmodel.rowCount()
  property bool canOpenFeaturesPage: false // if the advanced page for relations should be shown

  signal valueChanged( var value, bool isNull )
  signal featureLayerPairChanged()

  onFeatureLayerPairChanged: {
    // new feature layer pair, revert state and update delegate model
    textModeContainer.state = "initial"
    delegateModel.update()
  }

  RelationFeaturesModel {
    id: rmodel

    relation: associatedRelation
    parentFeatureLayerPair: featurePair

    onModelReset: root.linkedFeaturesCount = rowCount()
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
      else if ( textModeContainer.state === "expanded" ) {
        return true
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

  height: childrenRect.height

  Item {
    id: content

    height: childrenRect.height
    anchors {
      left: parent.left
      right: parent.right
    }

    // Text Mode Widget
    Rectangle {
      id: textModeContainer

      Component.onCompleted: delegateModel.update()

      states: [
        State {
          name: "initial"
          PropertyChanges {
            target: noOfFeaturesText
            visible: true
          }
        },
        State {
          name: "expanded"
          PropertyChanges {
            target: textModeContainer
            height: customStyle.fields.height * 3 // three rows
          }
          PropertyChanges {
            target: noOfFeaturesText
            visible: false
          }
        },
        State {
          name: "page"
//          PropertyChanges {
//            target: object

//          }
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

      visible: mode === "text"
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
          else if ( textModeContainer.state === "expanded" && root.canOpenFeaturesPage )
            textModeContrainer.state = "page"
        }
      }

      Flow {
        id: view

        anchors.fill: parent
        anchors.margins: customStyle.fields.sideMargin

        spacing: customStyle.group.spacing

        Repeater {
          model: delegateModel
        }
      }
    }

    // Photo Mode Widget
    Item {
      id: photoModeContainer

      visible: mode === "photo"
      anchors.fill: parent

      // todo: photo panel widget
    }
  }

  Component {
    id: textDelegate

    Item {
      id: textDelegateContainer

      property real maximumWidth: view.width
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
        onReleased: console.log("opening", model.FeatureId)
      }
    }
  }

  Component {
    id: photoDelegate

    Item {
      // todo: photo panel delegate
    }
  }
}
