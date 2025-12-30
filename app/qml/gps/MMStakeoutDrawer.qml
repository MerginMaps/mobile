/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Shapes

import mm 1.0 as MM
import MMInput

import "../components"
import "../map/components"
import "./components" as MMGpsComponents

MMDrawer {
  id: root

  property var mapCanvas

  property var targetPair: null
  property real remainingDistance: targetPair ? __inputUtils.distanceBetweenGpsAndFeature(
                                                  PositionKit.positionCoordinate,
                                                  targetPair,
                                                  mapCanvas.mapSettings ) : -1
  property var extent
  property real closeRangeModeDistanceThreshold: 1 // in metres
  property real targetReachedDistanceThreshold: 0.1 // in metres

  readonly property alias panelHeight: root.height

  signal panelHeightUpdated()
  signal stakeoutFinished()

  Component.onCompleted: {
    root.open()
  }

  function endStakeout() {
    if ( mapCanvas.state !== "stakeout" )
      return;

    stakeoutFinished()
  }

  function hide() {
    root.close()
  }

  function restore() {
    root.open()
  }

  Behavior on height {
    SequentialAnimation {
      PropertyAnimation { properties: "height"; easing.type: Easing.InOutQuad }
      ScriptAction { script: root.panelHeightUpdated() }
    }
  }

  StateGroup {
    id: distanceState

    states: [
      State {
        name: "longRange"
        when: root.remainingDistance >= root.closeRangeModeDistanceThreshold || root.remainingDistance < 0
      },
      State {
        name: "closeRange"
        when: root.remainingDistance >= 0 && root.remainingDistance < root.closeRangeModeDistanceThreshold
      }
    ]
  }

  modal: false

  closePolicy: Popup.CloseOnEscape // prevents the drawer closing while moving canvas

  dropShadow: true

  onClosed: root.endStakeout()

  drawerHeader.title: qsTr("Stake out")

  drawerContent: Column {
    id: mainColumn

    width: parent.width
    spacing: __style.margin12

    Row {
      width: parent.width

      MMGpsComponents.MMGpsDataText{
        width: ( parent.width + parent.spacing ) / 2

        title: qsTr( "Feature" )
        value: root.targetPair ? __inputUtils.featureTitle( root.targetPair, ActiveProject.qgsProject ) : ""
      }

      MMGpsComponents.MMGpsDataText{
        width: ( parent.width + parent.spacing ) / 2

        title: qsTr( "Distance" )
        value: remainingDistance >= 0 ?__inputUtils.formatDistanceInProjectUnit( remainingDistance, 2, ActiveProject.qgsProject ) : qsTr( "N/A" )
        alignmentRight: true
      }
    }

    MMLine {
      visible: distanceState.state === "closeRange"
    }

    ScrollView {
      id: gpsScrollView

      width: parent.width
      ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
      ScrollBar.vertical.policy: ScrollBar.AlwaysOff

      visible: distanceState.state === "closeRange"

      Column {
        width:parent.width
        anchors.fill: parent
        spacing: 0

        Item {
          id: closeRangeModeComponent

          height: 200 * __dp
          width: parent.width

          states: [
            State {
              name: "atTarget"
              when: root.remainingDistance < root.targetReachedDistanceThreshold
            },
            State {
              name: "notAtTarget"
              when: state !== "atTarget"
            }
          ]

          // enable antialiasing
          layer.enabled: true
          layer.samples: 4

          Shape {
            id: rootShape

            property real centerX: width / 2
            property real centerY: height / 2

            anchors.fill: parent

            ShapePath {
              strokeColor: closeRangeModeComponent.state === "notAtTarget" ? __style.greyColor : __style.lightGreenColor
              fillColor: closeRangeModeComponent.state === "notAtTarget" ? "white" : __style.lightGreenColor

              strokeWidth: 2 * __dp

              PathAngleArc {
                id: innerArc

                centerX: rootShape.centerX
                centerY: rootShape.centerY

                radiusX: outerArc.radiusX / 2
                radiusY: outerArc.radiusY / 2

                startAngle: 0
                sweepAngle: 360
              }
            }

            ShapePath {
              strokeColor: closeRangeModeComponent.state === "notAtTarget" ? __style.greyColor : __style.lightGreenColor
              fillColor: "transparent"

              strokeWidth: 2 * __dp

              PathAngleArc {
                id: outerArc

                property real outerRadius: rootShape.height / 2.5

                centerX: rootShape.centerX
                centerY: rootShape.centerY

                radiusX: outerRadius * __dp
                radiusY: outerRadius * __dp

                startAngle: 0
                sweepAngle: 360
              }
            }
          }

          // Target X icon
          MMIcon {
            source: __style.closeIcon
            size: rootShape.height / 12
            color: closeRangeModeComponent.state === "notAtTarget" ? __style.greyColor : __style.lightGreenColor
            x: rootShape.centerX - width / 2
            y: rootShape.centerY - height / 2
          }

          // Position indicator with direction
          Item {
            id: positionIndicatorItem

            MM.PositionDirection {
              id: positionDirection

              positionKit: PositionKit
              compass: MM.Compass { id: ccompass }
            }

            MMPositionMarker {
              id: positionMarker

              property real bearing: root.targetPair ? __inputUtils.angleBetweenGpsAndFeature(
                                                         PositionKit.positionCoordinate,
                                                         root.targetPair,
                                                         root.mapCanvas.mapSettings ) : 0

              xPos: ( rootShape.centerX + ( Math.sin( -bearing ) * root.remainingDistance ) * outerArc.outerRadius / root.closeRangeModeDistanceThreshold * __dp ) - width / 2
              yPos: ( rootShape.centerY + ( Math.cos( -bearing ) * root.remainingDistance ) * outerArc.outerRadius / root.closeRangeModeDistanceThreshold * __dp ) - height

              hasDirection: positionDirection.hasDirection

              direction: positionDirection.direction
              hasPosition: PositionKit.hasPosition

              horizontalAccuracy: PositionKit.horizontalAccuracy
              accuracyRingSize: 0 // do not show any accuracy ring in stakeout mode

              trackingMode: closeRangeModeComponent.state === "notAtTarget"

            }
          }
        }
      }
    }
  }
}
