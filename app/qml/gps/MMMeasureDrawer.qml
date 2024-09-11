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

  property bool canCloseShape: mapCanvas.mapToolComponent?.mapTool?.canCloseShape ?? false
  property bool closeShapeDone: mapCanvas.mapToolComponent?.mapTool?.closeShapeDone ?? false
  property bool canUndo: mapCanvas.mapToolComponent?.mapTool?.canUndo ?? false

  property string perimeter: __inputUtils.formatDistanceInProjectUnit( mapCanvas.mapToolComponent?.mapTool?.perimeter ?? 0, 1 )
  property string area: __inputUtils.formatAreaInProjectUnit( mapCanvas.mapToolComponent?.mapTool?.area ?? 0, 1 )

  signal measureFinished()
  signal measureDone()

  Component.onCompleted: root.open()

  modal: false
  interactive: false
  closePolicy: Popup.CloseOnEscape

  dropShadow: true

  onClosed: root.measureFinished()

  Behavior on implicitHeight {
    PropertyAnimation { properties: "implicitHeight"; easing.type: Easing.InOutQuad }
  }

  drawerHeader.title: qsTr( "Measurement" )

  drawerHeader.topLeftItemContent: MMButton {
    type: MMButton.Types.Primary
    text: closeShapeDone ? qsTr( "Repeat" ) : qsTr( "Undo" )
    iconSourceLeft: closeShapeDone ? __style.syncIcon : __style.undoIcon
    bgndColor: __style.lightGreenColor
    size: MMButton.Sizes.Small
    enabled: closeShapeDone || canUndo

    anchors {
      left: parent.left
      leftMargin: __style.pageMargins + __style.safeAreaLeft
      verticalCenter: parent.verticalCenter
    }

    onClicked: closeShapeDone ? root.mapCanvas.mapToolComponent.repeatMeasure() : root.mapCanvas.mapToolComponent.mapTool.removePoint()
  }

  drawerContent: Column {
    id: mainColumn

    width: parent.width
    spacing: __style.margin10

    Row {
      width: parent.width

      MMGpsComponents.MMGpsDataText{
        width: ( parent.width + parent.spacing ) / 2

        title: closeShapeDone ? qsTr( "Perimeter" ) : qsTr( "Length" )
        value: root.perimeter
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
        onClicked: canCloseShape ? root.mapCanvas.mapToolComponent.closeShape() : root.mapCanvas.mapToolComponent.addPoint()
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
