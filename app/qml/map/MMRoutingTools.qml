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
import QtQml.Models

import mm 1.0 as MM
import MMInput

import "../components"

/*
 * Online routing and navigation, enabled by the "routing" section of the project's field profile
 * (see RoutingController).
 *
 * - long press on the map offers directions to that point (openAt)
 * - the planning card lets the user change the start and destination (my location or a point
 *   picked on the map, see pickPoint), swap them and choose the mode of transport
 * - navigation follows the GPS position with the next instruction and the remaining distance and time
 *
 * The cards are at the top of the map, so the map buttons (e.g. quick capture) stay available.
 */

Item {
  id: root

  required property var map // MMMapCanvas

  readonly property bool enabled: controller.enabled
  readonly property bool navigating: controller.navigating
  //! 0 = not picking, 1 = picking the start, 2 = picking the destination
  property int pickingPoint: 0
  property bool panelOpen: false

  //! Asks the map to follow the GPS position (navigation started)
  signal followGpsRequested()

  function openAt( screenPoint ) {
    internal.pressedPoint = screenPoint
    promptLoader.active = true
  }

  function pickPoint( screenPoint ) {
    controller.setPointFromScreen( root.pickingPoint === 1, screenPoint )
    root.pickingPoint = 0
  }

  function close() {
    controller.clear()
    root.pickingPoint = 0
    root.panelOpen = false
  }

  MM.RoutingController {
    id: controller

    qgsProject: __activeProject.qgsProject
    mapSettings: root.map.mapSettings
    positionKit: PositionKit

    onRoutingFailed: function( message ) {
      __notificationModel.addError( message )
    }
  }

  Connections {
    target: __activeProject

    function onProjectReloaded( qgsProject ) {
      controller.reload()
      root.close()
    }
  }

  MMHighlight {
    width: root.map.width
    height: root.map.height
    mapSettings: root.map.mapSettings

    visible: controller.hasRoute
    geometry: controller.routeGeometry
    lineColor: internal.routeColor
    lineWidth: MMHighlight.LineWidths.Normal
  }

  MMHighlight {
    width: root.map.width
    height: root.map.height
    mapSettings: root.map.mapSettings

    visible: root.panelOpen && !controller.fromIsMyLocation
    geometry: controller.fromGeometry
    markerType: MMHighlight.MarkerTypes.Circle
    markerColor: __style.polarColor
    markerBorderColor: internal.routeColor
  }

  MMHighlight {
    width: root.map.width
    height: root.map.height
    mapSettings: root.map.mapSettings

    visible: root.panelOpen && !controller.toIsMyLocation
    geometry: controller.toGeometry
    markerColor: internal.routeColor
  }

  // planning card
  Rectangle {
    id: planningCard

    visible: root.panelOpen && !controller.navigating && root.pickingPoint === 0

    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
      topMargin: __style.safeAreaTop + __style.margin10
      leftMargin: __style.margin10
      rightMargin: __style.margin10
    }

    height: planningLayout.implicitHeight + 2 * __style.margin12
    radius: __style.radius12
    color: __style.polarColor

    layer.enabled: true
    layer.effect: MMShadow {}

    // keep touches on the card away from the map below
    MouseArea {
      anchors.fill: parent
    }

    ColumnLayout {
      id: planningLayout

      anchors {
        fill: parent
        margins: __style.margin12
      }

      spacing: __style.margin8

      RowLayout {
        Layout.fillWidth: true

        ColumnLayout {
          Layout.fillWidth: true
          spacing: __style.margin6

          Repeater {
            model: [ true, false ] // from, to

            RowLayout {
              Layout.fillWidth: true
              spacing: __style.margin8

              property bool isFrom: modelData
              property bool isMyLocation: isFrom ? controller.fromIsMyLocation : controller.toIsMyLocation

              MMText {
                text: isFrom ? qsTr( "From" ) : qsTr( "To" )
                font: __style.p6
                color: __style.darkGreenColor
                Layout.preferredWidth: 36 * __dp
              }

              MMText {
                Layout.fillWidth: true
                text: isMyLocation ? qsTr( "My location" ) : qsTr( "Point on the map" )
                font: __style.t3
                elide: Text.ElideRight
              }

              MMButton {
                text: qsTr( "Pick" )
                type: MMButton.Types.Secondary
                size: MMButton.Sizes.ExtraSmall
                onClicked: root.pickingPoint = isFrom ? 1 : 2
              }

              MMButton {
                text: qsTr( "GPS" )
                type: MMButton.Types.Tertiary
                size: MMButton.Sizes.ExtraSmall
                visible: !isMyLocation
                onClicked: controller.setPointToMyLocation( isFrom )
              }
            }
          }
        }

        MMRoundButton {
          iconSource: __style.arrowDownIcon
          bgndColor: __style.lightGreenColor
          onClicked: controller.swapPoints()
        }
      }

      // mode of transport
      RowLayout {
        Layout.fillWidth: true
        spacing: __style.margin6

        Repeater {
          model: controller.modes

          MMButton {
            Layout.fillWidth: true
            text: modelData.label
            size: MMButton.Sizes.ExtraSmall
            type: controller.modeIndex === index ? MMButton.Types.Primary : MMButton.Types.Secondary
            onClicked: controller.modeIndex = index
          }
        }
      }

      MMText {
        Layout.fillWidth: true
        font: __style.t2
        text: {
          if ( controller.busy ) return qsTr( "Calculating the route…" )
          if ( controller.hasRoute ) return qsTr( "%1 · %2 · arrive %3" ).arg( controller.durationText ).arg( controller.distanceText ).arg( controller.arrivalText )
          return ""
        }
        visible: text !== ""
      }

      RowLayout {
        Layout.fillWidth: true
        spacing: __style.margin8

        MMButton {
          Layout.fillWidth: true
          text: qsTr( "Start" )
          enabled: controller.hasRoute && !controller.busy
          onClicked: {
            controller.startNavigation()
            root.followGpsRequested()
          }
        }

        MMButton {
          text: qsTr( "Close" )
          type: MMButton.Types.Tertiary
          onClicked: root.close()
        }
      }
    }
  }

  // picking a point on the map
  Rectangle {
    visible: root.pickingPoint > 0

    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
      topMargin: __style.safeAreaTop + __style.margin10
      leftMargin: __style.margin10
      rightMargin: __style.margin10
    }

    height: pickLayout.implicitHeight + 2 * __style.margin12
    radius: __style.radius12
    color: __style.polarColor

    layer.enabled: true
    layer.effect: MMShadow {}

    // keep touches on the card away from the map below
    MouseArea {
      anchors.fill: parent
    }

    RowLayout {
      id: pickLayout

      anchors {
        fill: parent
        margins: __style.margin12
      }

      MMText {
        Layout.fillWidth: true
        text: root.pickingPoint === 1 ? qsTr( "Tap the map to choose the start" ) : qsTr( "Tap the map to choose the destination" )
        font: __style.t3
        wrapMode: Text.WordWrap
      }

      MMButton {
        text: qsTr( "Cancel" )
        type: MMButton.Types.Tertiary
        size: MMButton.Sizes.ExtraSmall
        onClicked: root.pickingPoint = 0
      }
    }
  }

  // navigation banner
  Rectangle {
    visible: controller.navigating

    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
      topMargin: __style.safeAreaTop + __style.margin10
      leftMargin: __style.margin10
      rightMargin: __style.margin10
    }

    height: navigationLayout.implicitHeight + 2 * __style.margin12
    radius: __style.radius12
    color: __style.forestColor

    layer.enabled: true
    layer.effect: MMShadow {}

    // keep touches on the card away from the map below
    MouseArea {
      anchors.fill: parent
    }

    ColumnLayout {
      id: navigationLayout

      anchors {
        fill: parent
        margins: __style.margin12
      }

      spacing: __style.margin6

      RowLayout {
        Layout.fillWidth: true
        spacing: __style.margin12

        MMText {
          text: controller.distanceToInstructionText
          font: __style.t1
          color: __style.polarColor
          visible: text !== ""
        }

        MMText {
          Layout.fillWidth: true
          text: controller.instruction
          font: __style.t2
          color: __style.polarColor
          wrapMode: Text.WordWrap
          maximumLineCount: 3
        }
      }

      RowLayout {
        Layout.fillWidth: true

        MMText {
          Layout.fillWidth: true
          text: qsTr( "%1 · %2 · arrive %3" ).arg( controller.durationText ).arg( controller.distanceText ).arg( controller.arrivalText )
          font: __style.p5
          color: __style.polarColor
        }

        MMButton {
          text: controller.arrived ? qsTr( "Done" ) : qsTr( "End" )
          size: MMButton.Sizes.ExtraSmall
          onClicked: root.close()
        }
      }
    }
  }

  // shown on long press on the map
  Loader {
    id: promptLoader

    active: false
    sourceComponent: promptComponent
  }

  Component {
    id: promptComponent

    MMListDrawer {
      id: prompt

      drawerHeader.title: qsTr( "Navigation" )

      Component.onCompleted: open()
      onClosed: promptLoader.active = false

      list.model: ObjectModel {
        MMListDelegate {
          text: qsTr( "Directions to here" )
          leftContent: MMIcon { source: __style.directionsIcon }
          hasLine: false

          onClicked: {
            prompt.close()
            root.panelOpen = true
            controller.routeTo( internal.pressedPoint )
          }
        }
      }
    }
  }

  QtObject {
    id: internal

    property point pressedPoint
    readonly property color routeColor: "#1A73E8"
  }
}
