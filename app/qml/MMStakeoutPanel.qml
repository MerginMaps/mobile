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

import "components"
import "gps"
import ".."

import lc 1.0

Drawer {
  id: root

  property int gpsIconHeight: 70
  property int gpsIconWidth: 58

  property var mapCanvas

  property var targetPair: null
  property real remainingDistance: targetPair ? __inputUtils.distanceBetweenGpsAndFeature(
                                                  __positionKit.positionCoordinate,
                                                  targetPair,
                                                  mapCanvas.mapSettings ) : -1

  property var extent

  property real closeRangeModeDistanceThreshold: 1 // in metres
  property real targetReachedDistanceThreshold: 0.1 // in metres

  readonly property alias panelHeight: root.height

  // Intentionally create additional signal that signalizes when stakeout panel changes its height
  // panelHeightUpdated is emitted after animation for panel height is finished! panelHeight property contains also
  // intermediary values during animation
  signal panelHeightUpdated()

  signal autoFollowClicked()
  signal stakeoutFinished()

  Component.onCompleted: {
    // stakeout starts
    root.open()
  }

  function endStakeout() {
    if ( mapCanvas.state !== "stakeout" )
      return;

    root.close()
    stakeoutFinished()
  }

  function hide() {
    root.close()
  }

  function restore() {
    root.open()
  }

  height: ApplicationWindow.window.height
  width: ApplicationWindow.window.width

  Behavior on height {
    SequentialAnimation {
      PropertyAnimation { properties: "height"; easing.type: Easing.InOutQuad }
      ScriptAction { script: root.panelHeightUpdated() }
    }
  }

  Item {
    id: drawerContent
    states: [
        State {
            name: "longRange"
            when: remainingDistance >= closeRangeModeDistanceThreshold || remainingDistance < 0
            PropertyChanges {
                target: drawer
                height: Math.max( 2 * InputStyle.rowHeight, root.height / 6 )
            }
        },
        State {
            name: "closeRange"
            when: remainingDistance >= 0 && remainingDistance < closeRangeModeDistanceThreshold
            PropertyChanges {
                target: drawer
                height: Math.max( 4 * InputStyle.rowHeight, root.height / 2 )
            }
        }
    ]
  }

  modal: false
  edge: Qt.BottomEdge
  interactive: false // prevents closing by swiping the window down
  dragMargin: 0 // prevents opening the root by dragging.
  //closePolicy: Popup.NoAutoClose

  Item {
    // back handler
    focus: true

    Keys.onReleased: function( event ) {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
        event.accepted = true;
        endStakeout()
      }
    }
  }

  Rectangle {
    anchors.fill: parent
    color: __style.whiteColor
    anchors.topMargin: -radius
    radius: __style.inputRadius
  }

  ColumnLayout {

    id: mainColumn

    width: parent.width
    spacing: 40 * __dp

    MMHeader {

      id: header

      rightMarginShift: 0
      backVisible: false

      title: qsTr("Stake out")
      titleFont: __style.t2

      MMRoundButton {
        id: backBtn

        anchors.right: parent.right
        anchors.rightMargin: __style.pageMargins
        anchors.verticalCenter: parent.verticalCenter

        iconSource: __style.closeIcon
        iconColor: __style.forestColor

        bgndColor: __style.lightGreenColor
        bgndHoverColor: __style.mediumGreenColor

        onClicked: root.visible = false
      }
    }

    ScrollView {
      id: scrollView

      Layout.fillWidth: true
      Layout.leftMargin: __style.pageMargins
      Layout.rightMargin: __style.pageMargins
      Layout.maximumWidth: __style.maxPageWidth
      Layout.alignment: Qt.AlignHCenter
      Layout.preferredHeight: window.height - header.height
      contentWidth: availableWidth
      contentHeight: scrollColumn.childrenRect.height

      ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
      ScrollBar.vertical.policy: ScrollBar.AlwaysOff

      Column{
        id: scrollColumn

        width: parent.width
        spacing: 0

        Row {
          width: parent.width
          height: 67 * __dp

          MMGpsDataText{
            titleText: qsTr( "Feature" )
            descriptionText: root.targetPair ? __inputUtils.featureTitle( root.targetPair, __activeProject.qgsProject ) : ""
          }

          MMGpsDataText{
            titleText: qsTr( "Distance" )
            descriptionText: remainingDistance >= 0 ?__inputUtils.formatDistanceInProjectUnit( remainingDistance, 2 ) : "N/A"
            alignmentRight: true
          }
        }

        MMLine {
          visible: drawerContent.state === "closeRange"
        }

        // Position indicator with direction
        Item {
          width: parent.width
          height: __style.pageMargins
        }

        Item {
          id: closeRangeModeComponent

          width: parent.width
          height: childrenRect.height

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

          //state: "notAtTarget"
          //state: "atTarget"

          visible: drawerContent.state === "closeRange"

          // enable antialiasing
          layer.enabled: true
          layer.samples: 4

          Shape {
            id: rootShape

            property real centerX: width / 2
            property real centerY: height / 2

            anchors.fill: parent

            ShapePath {
              strokeColor: closeRangeModeComponent.state === "notAtTarget" ? __style.greyColor : __style.positiveColor
              fillColor: closeRangeModeComponent.state === "notAtTarget" ? __style.whiteColor : __style.positiveColor

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
              strokeColor: closeRangeModeComponent.state === "notAtTarget" ? __style.greyColor : __style.positiveColor
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

          Symbol {
            source: __style.stakeOutDotIcon
            //iconColor: closeRangeModeComponent.state === "notAtTarget" ? InputStyle.panelBackgroundDarker : InputStyle.fontColorBright
            iconSize: rootShape.height / 12
            x: rootShape.centerX - width / 2
            y: rootShape.centerY - height / 2
          }

          Item {
            id: positionMarker
            width: parent.width
            height: childrenRect.height

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

                anchors.horizontalCenter: parent.horizontalCenter
                source: __style.gpsDirectionIcon
                fillMode: Image.PreserveAspectFit
                rotation: positionDirection.direction
                transformOrigin: Item.Bottom
                width: root.gpsIconWidth
                height: root.gpsIconHeight
                smooth: true
                visible: __positionKit.hasPosition && positionDirection.hasDirection

                x: ( rootShape.centerX + ( Math.sin( -bearing ) * root.remainingDistance ) * outerArc.outerRadius / root.closeRangeModeDistanceThreshold * __dp ) - width / 2
                y: ( rootShape.centerY + ( Math.cos( -bearing ) * root.remainingDistance ) * outerArc.outerRadius / root.closeRangeModeDistanceThreshold * __dp ) - height

                Behavior on rotation { RotationAnimation { properties: "rotation"; direction: RotationAnimation.Shortest; duration: 500 }}
            }

            //Prob not needed
            // Image {
            //     source: __positionKit.hasPosition ? InputStyle.gpsMarkerPositionIcon : InputStyle.gpsMarkerNoPositionIcon
            //     visible: __positionKit.hasPosition
            //     fillMode: Image.PreserveAspectFit
            //     width: InputStyle.rowHeightHeader / 2
            //     height: width
            //     smooth: true
            //     x: ( rootShape.centerX + ( Math.sin( -direction.bearing ) * root.remainingDistance ) * outerArc.outerRadius / root.closeRangeModeDistanceThreshold * __dp ) - width / 2
            //     y: ( rootShape.centerY + ( Math.cos( -direction.bearing ) * root.remainingDistance ) * outerArc.outerRadius / root.closeRangeModeDistanceThreshold * __dp ) - height / 2
            // }
          }
        }
      }
    }
  }
}

