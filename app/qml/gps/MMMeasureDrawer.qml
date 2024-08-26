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

  readonly property alias panelHeight: root.height

  property bool canCloseShape: false
  property bool closeShapeDone: false
  property bool canUndo: false

  property string length: qsTr( "0.0 m" )
  property string perimeter: qsTr( "0.0 m" )
  property string area: qsTr( "0.0 m²" )

  signal addMeasurePoint()
  signal measureFinished()
  signal measureDone()
  signal closeShape()
  signal undo()
  signal repeat()

  Component.onCompleted: {
    root.open()
  }

  modal: false

  closePolicy: Popup.CloseOnEscape

  dropShadow: true

  onClosed: root.measureFinished()

  leftButtonText: closeShapeDone ? qsTr( "Repeat" ) : qsTr( "Undo" )
  leftButtonIcon: closeShapeDone ? __style.syncIcon : __style.undoIcon
  leftButtonType: MMButton.Types.Primary
  leftButtonEnabled: closeShapeDone || canUndo
  onLeftButtonClicked: closeShapeDone ? root.repeatMeasure() : root.undo()

  drawerHeader.title: qsTr( "Measurement" )

  drawerContent: Column {
    id: mainColumn

    width: parent.width
    spacing: __style.margin40

    Row {
      width: parent.width

      MMGpsComponents.MMGpsDataText{
        width: ( parent.width + parent.spacing ) / 2

        title: closeShapeDone ? qsTr( "Perimeter" ) : qsTr( "Length" )
        value: closeShapeDone ? root.perimeter : root.length
      }

      MMGpsComponents.MMGpsDataText{
        width: ( parent.width + parent.spacing ) / 2

        title: qsTr( "Area" )
        value: root.area
        alignmentRight: true
        visible: closeShapeDone
      }
    }

    Row {
      width: parent.width
      spacing: __style.margin12
      visible: !root.closeShapeDone

      MMButton {
        text: root.canCloseShape ? qsTr( "Close shape" ) : qsTr( "Add point" )
        iconSourceLeft: canCloseShape ? __style.closeShapeIcon : __style.plusIcon
        onClicked: canCloseShape ? root.closeShape() : root.addMeasurePoint()
      }

      MMButton {
        type: MMButton.Types.Secondary
        text: qsTr( "Done" )
        iconSourceLeft: __style.doneCircleIcon
        onClicked: root.measureDone()
      }
    }
  }

  function endMeasurement()
  {
    if ( mapCanvas.state !== "measure" )
      return;

    measureFinished()
  }

  function restore()
  {
    root.open()
  }

  function repeatMeasure()
  {
    root.closeShapeDone = false
    root.canCloseShape = false
    root.repeat()
  }
}
