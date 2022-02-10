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

  property var navigationTargetPair: null
  property real featureToGpsDistance: navigationTargetPair ? __inputUtils.distanceBetweenGpsAndFeature(
                                                                  __positionKit.positionCoordinate,
                                                                  navigationTargetPair,
                                                                  mapCanvas.mapSettings ) : -1

  // Determines if canvas is auto centered to stakout line
  property bool autoFollow: true

  property var calculatedNavigationExtent

  property real distanceThresholdToShortMode: 1 // in metres
  property real distanceThresholdToFinishNavigation: 0.1 // in metres

  readonly property alias panelHeight: drawer.height

  signal navigationFinished()

  Component.onCompleted: {
    // stakeout starts
    drawer.open()
  }

  function endStakeout() {
    if ( mapCanvas.state !== "navigation" )
      return;

    drawer.close()
    navigationFinished()
  }

  function hide() {
    drawer.close()
  }

  function restore() {
    drawer.open()
  }

  function updateNavigation() {
    if ( mapCanvas.state !== "navigation" )
      return;

    if ( root.navigationTargetPair )
    {
      // reserve some space around canvas so that position marker is always visible in autofollow mode
      let previewMargin = direction.height * 3
      let stakoutPanelHeightRatio = ( drawer.height + previewMargin ) / mapCanvas.height

      //
      // This is ugly .. it should ideally be computed in MapWrapper, not here.
      // It needs to be revisited when QML refactoring will take place (separation of project to new component)
      //
      root.calculatedNavigationExtent =  __inputUtils.navigationFeatureExtent(
            root.navigationTargetPair,
            __positionKit.positionCoordinate,
            mapCanvas.mapSettings,
            stakoutPanelHeightRatio
            );

      if ( autoFollow )
      {
        mapCanvas.mapSettings.extent = calculatedNavigationExtent;
      }
    }
  }

  onAutoFollowChanged: updateNavigation()

  states: [
    State {
      name: "long"
      when: featureToGpsDistance >= distanceThresholdToShortMode
      PropertyChanges {
        target: drawer
        height: root.height / 6
      }
    },
    State {
      name: "short"
      when: featureToGpsDistance >= 0 && featureToGpsDistance < distanceThresholdToShortMode
      PropertyChanges {
        target: drawer
        height: root.height / 2
      }
    }
  ]

  Connections {
    target: mapCanvas.mapSettings
    onExtentChanged: {
      if ( mapCanvas.state === "navigation" && mapCanvas.mapSettings.extent !== calculatedNavigationExtent )
        autoFollow = false;
    }
  }

  Connections {
    target: __positionKit
    onPositionChanged: {
      if ( mapCanvas.state === "navigation" && navigationTargetPair )
        updateNavigation();
    }
  }

  onStateChanged: updateNavigation()

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
      PropertyAnimation { properties: "height"; easing.type: Easing.InOutQuad }
    }

    width: parent.width

    modal: false
    edge: Qt.BottomEdge

    dragMargin: 0 // prevents opening the drawer by dragging.
    closePolicy: Popup.CloseOnEscape // prevents the drawer closing while moving canvas

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

          visible: root.autoFollow === false

          Components.Symbol {
            source: InputStyle.gpsFixedIcon
            iconSize: parent.height / 2
            anchors.centerIn: parent
          }

          MouseArea {
            anchors.fill: parent
            onClicked: root.autoFollow = true
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
          Layout.preferredHeight: root.state === "long" ? parent.height : ( parent.width * 1/6 )
          Layout.preferredWidth: parent.width

          Components.TextRowWithTitle {
            id: featuretitle

            height: parent.height
            width: parent.width / 2

            titleText: qsTr( "Feature" )
            text: root.navigationTargetPair ? __inputUtils.featureTitle( root.navigationTargetPair, __loader.project ) : ""
          }

          Components.TextRowWithTitle {
            id: distancetext

            height: parent.height
            width: parent.width / 2

            titleText: qsTr( "Distance" )
            text: __inputUtils.formatNumber( featureToGpsDistance, 2 ) + " m"
          }
        }

        Item {
          id: navigationComponent

          Layout.fillHeight: true
          Layout.fillWidth: true

          states: [
            State {
              name: "atTarget"
              when: root.featureToGpsDistance < root.distanceThresholdToFinishNavigation
            },
            State {
              name: "notAtTarget"
              when: state != "atTarget"
            }
          ]

          onStateChanged: root.updateNavigation()

          state: "notAtTarget"

          visible: root.state === "short"

          // enable antialiasing
          layer.enabled: true
          layer.samples: 4

          Shape {
            id: rootShape

            property real centerX: width / 2
            property real centerY: height / 2

            anchors.fill: parent

            ShapePath {
              strokeColor: navigationComponent.state === "notAtTarget" ? InputStyle.labelColor : InputStyle.fontColorBright
              fillColor: navigationComponent.state === "notAtTarget" ? "white" : InputStyle.fontColorBright

              strokeWidth: 2 * __dp

              PathAngleArc {
                id: innerArc

                centerX: rootShape.centerX
                centerY: rootShape.centerY

                radiusX: 50 * __dp
                radiusY: 50 * __dp

                startAngle: 0
                sweepAngle: 360
              }
            }

            ShapePath {
              strokeColor: navigationComponent.state === "notAtTarget" ? InputStyle.labelColor : InputStyle.fontColorBright
              fillColor: "transparent"

              strokeWidth: 2 * __dp

              PathAngleArc {
                id: outerArc

                centerX: rootShape.centerX
                centerY: rootShape.centerY

                radiusX: 100 * __dp
                radiusY: 100 * __dp

                startAngle: 0
                sweepAngle: 360
              }
            }
          }

          // Target X icon
          Components.Symbol {
            source: InputStyle.noIcon
            iconColor: navigationComponent.state === "notAtTarget" ? InputStyle.panelBackgroundDarker : InputStyle.fontColorBright
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

                property real bearing: root.navigationTargetPair ? __inputUtils.angleBetweenGpsAndFeature(
                                                                        __positionKit.positionCoordinate,
                                                                        root.navigationTargetPair,
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
                  *   distance of the outer circle in metres (distanceThresholdToShortMode)
                  */
                x: ( rootShape.centerX + ( Math.sin( -bearing ) * root.featureToGpsDistance ) * outerArc.radiusX / root.distanceThresholdToShortMode * __dp ) - width / 2
                y: ( rootShape.centerY + ( Math.cos( -bearing ) * root.featureToGpsDistance ) * outerArc.radiusX / root.distanceThresholdToShortMode * __dp ) - height

                Behavior on rotation { RotationAnimation { properties: "rotation"; direction: RotationAnimation.Shortest; duration: 500 }}
            }

            Image {
                source: __positionKit.hasPosition ? InputStyle.gpsMarkerPositionIcon : InputStyle.gpsMarkerNoPositionIcon
                visible: __positionKit.hasPosition
                fillMode: Image.PreserveAspectFit
                width: InputStyle.rowHeightHeader / 2
                height: width
                smooth: true
                x: ( rootShape.centerX + ( Math.sin( -direction.bearing ) * root.featureToGpsDistance ) * outerArc.radiusX / root.distanceThresholdToShortMode * __dp ) - width / 2
                y: ( rootShape.centerY + ( Math.cos( -direction.bearing ) * root.featureToGpsDistance ) * outerArc.radiusX / root.distanceThresholdToShortMode * __dp ) - height / 2
            }
          }
        }
      }
    }
  }
}
