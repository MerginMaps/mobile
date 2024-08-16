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

import "../components"
import "../map/components"
import "./components" as MMGpsComponents

MMDrawer {
  id: root

  property var mapCanvas

  property var targetPair: null
  // property real remainingDistance: targetPair ? __inputUtils.distanceBetweenGpsAndFeature(
  //                                                 __positionKit.positionCoordinate,
  //                                                 targetPair,
  //                                                 mapCanvas.mapSettings ) : -1
  property bool closeShapeActive: true
  property bool closeShapeDone: true

  readonly property alias panelHeight: root.height

  signal panelHeightUpdated()
  signal addMeasurePoint()
  signal measureFinished()
  signal measureDone()

  Component.onCompleted: {
    root.open()
  }

  function endMeasurement() {
    if ( mapCanvas.state !== "measure" )
      return;

    measureFinished()
  }

  function hide() {
    root.close()
  }

  function restore() {
    root.open()
  }

  function closeShape()
  {
    console.log(" close shape clicked ")
    root.closeShapeActive = false
  }

  function addPoint()
  {
    root.addMeasurePoint()
    console.log(" point added ")
  }

  function repeatMeasure()
  {
    console.log(" repeat measure ")
  }

  modal: false

  closePolicy: Popup.CloseOnEscape // prevents the drawer closing while moving canvas

  dropShadow: true

  //onClosed: root.endStakeout()

  leftButtonText: closeShapeDone ? qsTr( "Repeat" ) : qsTr( "Undo" )
  leftButtonIcon: closeShapeDone ? __style.syncIcon : __style.undoIcon
  leftButtonType: MMButton.Types.Primary
  onLeftButtonClicked: console.log("onLeftButtonClicked")

  drawerHeader.title: qsTr( "Measure" )

  drawerContent: Column {
    id: mainColumn

    width: parent.width
    spacing: __style.margin40

    Row {
      width: parent.width

      MMGpsComponents.MMGpsDataText{
        width: ( parent.width + parent.spacing ) / 2

        title: closeShapeActive ? qsTr( "Perimeter" ) : qsTr( "Length" )
        value: "58.4 m" //remainingDistance >= 0 ?__inputUtils.formatDistanceInProjectUnit( remainingDistance, 2 ) : qsTr( "N/A" )
      }

      MMGpsComponents.MMGpsDataText{
        width: ( parent.width + parent.spacing ) / 2

        title: qsTr( "Area" )
        value: "0.00 m"
        alignmentRight: true
        visible: closeShapeActive
      }
    }

    Row {
      width: parent.width
      spacing: __style.margin12

      MMButton {
        text: closeShapeActive ? qsTr( "Close shape" ) : qsTr( "Add point" )
        iconSourceLeft: closeShapeActive ? __style.closeShapeIcon : __style.plusIcon
        onClicked: closeShapeActive ? root.closeShape() : root.addPoint()
      }

      MMButton {
        type: MMButton.Types.Secondary
        text: qsTr( "Done" )
        iconSourceLeft: __style.doneCircleIcon
        onClicked: root.measureDone()
      }
    }
  }
}
