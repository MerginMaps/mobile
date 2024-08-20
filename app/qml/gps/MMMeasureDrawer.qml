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

  property bool closeShapeActive: false
  property bool closeShapeDone: false

  property string length: qsTr( "N/A" )
  property string perimeter: qsTr( "N/A" )
  property string area: qsTr( "N/A" )

  signal addMeasurePoint()
  signal measureFinished()
  signal measureDone()
  signal closeShape()
  signal undo()
  signal repeat()

  Component.onCompleted: {
    root.open()
  }

  function endMeasurement() {
    if ( mapCanvas.state !== "measure" )
      return;

    measureFinished()
  }

  function restore() {
    root.open()
  }

  modal: false

  closePolicy: Popup.CloseOnEscape

  dropShadow: true

  onClosed: root.measureFinished()

  leftButtonText: closeShapeDone ? qsTr( "Repeat" ) : qsTr( "Undo" )
  leftButtonIcon: closeShapeDone ? __style.syncIcon : __style.undoIcon
  leftButtonType: MMButton.Types.Primary
  onLeftButtonClicked: closeShapeDone ? root.repeat() : root.undo()

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
        value: closeShapeActive ? root.perimeter : root.length
      }

      MMGpsComponents.MMGpsDataText{
        width: ( parent.width + parent.spacing ) / 2

        title: qsTr( "Area" )
        value: root.area
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
        onClicked: closeShapeActive ? root.closeShape() : root.addMeasurePoint()
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
