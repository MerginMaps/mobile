/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.0
import QtQuick.Shapes 1.14

import "."  // import InputStyle singleton
import "./components" as Components
import "./map" as Map

import lc 1.0

Item {
  id: root

  property var mapCanvas

  property var targetPair: null
  property real remainingDistance: targetPair ? __inputUtils.distanceBetweenGpsAndFeature(
                                                                  __positionKit.positionCoordinate,
                                                                  targetPair,
                                                                  mapCanvas.mapSettings ) : -1

  property var extent

  property real closeRangeModeDistanceThreshold: 1 // in metres
  property real targetReachedDistanceThreshold: 0.1 // in metres

  readonly property alias panelHeight: drawer.height
  // Intentionally create additional signal that signalizes when stakeout panel changes its height
  // panelHeightUpdated is emitted after animation for panel height is finished! panelHeight property contains also
  // intermediary values during animation
  signal panelHeightUpdated()

  signal autoFollowClicked()
  signal stakeoutFinished()

  Component.onCompleted: {
    // stakeout starts
    drawer.open()
  }

  function endStakeout() {
    if ( mapCanvas.state !== "stakeout" )
      return;

    drawer.close()
    stakeoutFinished()
  }

  function hide() {
    drawer.close()
  }

  function restore() {
    drawer.open()
  }

  states: [
    State {
      name: "longRange"
      when: remainingDistance >= closeRangeModeDistanceThreshold || remainingDistance < 0
      PropertyChanges {
        target: drawer
        height: root.height / 6
      }
    },
    State {
      name: "closeRange"
      when: remainingDistance >= 0 && remainingDistance < closeRangeModeDistanceThreshold
      PropertyChanges {
        target: drawer
        height: root.height / 2
      }
    }
  ]

  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true;
      endStakeout()
    }
  }

  focus: true

  Drawer {
    id: drawer

    Behavior on height {
      SequentialAnimation {
        PropertyAnimation { properties: "height"; easing.type: Easing.InOutQuad }
        ScriptAction { script: root.panelHeightUpdated() }
      }
    }

    width: parent.width

    modal: false
    edge: Qt.BottomEdge
    interactive: false // prevents closing by swiping the window down
    dragMargin: 0 // prevents opening the drawer by dragging.
    closePolicy: Popup.NoAutoClose

    Item {
      // back handler
      focus: true

      Keys.onReleased: {
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
          event.accepted = true;
          endStakeout()
        }
      }
    }

    Rectangle {
      anchors.fill: parent
      color: InputStyle.clrPanelMain
    }

    Column {
      id: body

      anchors {
        fill: parent
        leftMargin: InputStyle.panelMargin
        rightMargin: InputStyle.panelMargin
      }

      // Header - always there
      RowLayout {
        id: header

        width: parent.width
        height: InputStyle.rowHeight

        spacing: 0

        Text {
          Layout.preferredHeight: parent.height
          Layout.fillWidth: true

          text: qsTr( "Stake out" )

          font.pixelSize: InputStyle.fontPixelSizeBig

          font.bold: true
          elide: Qt.ElideRight
          color: InputStyle.fontColor
          horizontalAlignment: Text.AlignLeft
          verticalAlignment: Text.AlignVCenter
        }

        Item {
          id: recenterbtn

          Layout.preferredHeight: parent.height
          Layout.preferredWidth: parent.height

          visible: mapCanvas.autoFollowStakeoutPath === false

          Components.Symbol {
            source: InputStyle.gpsFixedIcon
            iconSize: parent.height / 2
            anchors.centerIn: parent
          }

          MouseArea {
            anchors.fill: parent
            onClicked: root.autoFollowClicked()
          }
        }

        Item {
          id: closebtn

          Layout.preferredHeight: parent.height
          Layout.preferredWidth: parent.height

          Components.Symbol {
            source: InputStyle.noIcon
            iconSize: parent.height / 2
            anchors.centerIn: parent
          }

          MouseArea {
            anchors.fill: parent
            onClicked: endStakeout()
          }
        }
      }

      // Separator
      Rectangle {
          width: parent.width
          height: 1
          color: InputStyle.fontColor
      }

      // Content - changes based on state
      ColumnLayout {
        id: content

        height: parent.height - header.height
        width: parent.width

        Row {
          Layout.preferredHeight: root.state === "longRange" ? parent.height : ( parent.width * 1/6 )
          Layout.preferredWidth: parent.width

          Components.TextRowWithTitle {
            id: featuretitle

            height: parent.height
            width: parent.width / 2

            titleText: qsTr( "Feature" )
            text: root.targetPair ? __inputUtils.featureTitle( root.targetPair, __loader.project ) : ""
          }

          Components.TextRowWithTitle {
            id: distancetext

            height: parent.height
            width: parent.width / 2

            titleText: qsTr( "Distance" )
            text: remainingDistance >= 0 ?__inputUtils.formatNumber( remainingDistance, 2 ) + " m" : "N/A m"
          }
        }

        Item {
          id: closeRangeModeComponent

          Layout.fillHeight: true
          Layout.fillWidth: true

          states: [
            State {
              name: "atTarget"
              when: root.remainingDistance < root.targetReachedDistanceThreshold
            },
            State {
              name: "notAtTarget"
              when: state != "atTarget"
            }
          ]

          state: "notAtTarget"

          visible: root.state === "closeRange"

          // enable antialiasing
          layer.enabled: true
          layer.samples: 4

          Shape {
            id: rootShape

            property real centerX: width / 2
            property real centerY: height / 2

            anchors.fill: parent

            ShapePath {
              strokeColor: closeRangeModeComponent.state === "notAtTarget" ? InputStyle.labelColor : InputStyle.fontColorBright
              fillColor: closeRangeModeComponent.state === "notAtTarget" ? "white" : InputStyle.fontColorBright

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
              strokeColor: closeRangeModeComponent.state === "notAtTarget" ? InputStyle.labelColor : InputStyle.fontColorBright
              fillColor: "transparent"

              strokeWidth: 2 * __dp

              PathAngleArc {
                id: outerArc

                property real outerRadius: {
                  if ( rootShape.height / 2.5 < 100 )
                  {
                    return 100 // minimum height
                  }
                  return rootShape.height / 2.5
                }

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
          Components.Symbol {
            source: InputStyle.noIcon
            iconColor: closeRangeModeComponent.state === "notAtTarget" ? InputStyle.panelBackgroundDarker : InputStyle.fontColorBright
            iconSize: rootShape.height / 12
            x: rootShape.centerX - width / 2
            y: rootShape.centerY - height / 2
          }

          // Position indicator with direction
          Item {
            id: positionMarker

            PositionDirection {
              id: positionDirection

              positionKit: __positionKit
              compass: Compass { id: ccompass }
            }

            Image {
                id: direction

                property real bearing: root.targetPair ? __inputUtils.angleBetweenGpsAndFeature(
                                                                        __positionKit.positionCoordinate,
                                                                        root.targetPair,
                                                                        root.mapCanvas.mapSettings ) : 0

                source: InputStyle.gpsDirectionIcon
                fillMode: Image.PreserveAspectFit
                rotation: positionDirection.direction
                transformOrigin: Item.Bottom
                width: InputStyle.rowHeightHeader
                height: width
                smooth: true
                visible: __positionKit.hasPosition && positionDirection.hasDirection

                /**
                  * Formula to calculate GPS position in the short-range window goes like this:
                  *   center of the window +
                  *   sin<or cos> of angle between GPS position and the target feature *
                  *   distance to the feature *
                  *   scale by size of the outer circle /
                  *   distance of the outer circle in metres (closeRangeModeDistanceThreshold)
                  */
                x: ( rootShape.centerX + ( Math.sin( -bearing ) * root.remainingDistance ) * outerArc.outerRadius / root.closeRangeModeDistanceThreshold * __dp ) - width / 2
                y: ( rootShape.centerY + ( Math.cos( -bearing ) * root.remainingDistance ) * outerArc.outerRadius / root.closeRangeModeDistanceThreshold * __dp ) - height

                Behavior on rotation { RotationAnimation { properties: "rotation"; direction: RotationAnimation.Shortest; duration: 500 }}
            }

            Image {
                source: __positionKit.hasPosition ? InputStyle.gpsMarkerPositionIcon : InputStyle.gpsMarkerNoPositionIcon
                visible: __positionKit.hasPosition
                fillMode: Image.PreserveAspectFit
                width: InputStyle.rowHeightHeader / 2
                height: width
                smooth: true
                x: ( rootShape.centerX + ( Math.sin( -direction.bearing ) * root.remainingDistance ) * outerArc.outerRadius / root.closeRangeModeDistanceThreshold * __dp ) - width / 2
                y: ( rootShape.centerY + ( Math.cos( -direction.bearing ) * root.remainingDistance ) * outerArc.outerRadius / root.closeRangeModeDistanceThreshold * __dp ) - height / 2
            }
          }
        }
      }
    }
  }
}
