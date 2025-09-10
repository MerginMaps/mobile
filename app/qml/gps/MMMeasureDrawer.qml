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

  property var mapTool

  readonly property alias panelHeight: root.height

  property bool canCloseShape: mapTool?.canCloseShape ?? false
  property bool canUndo: mapTool?.canUndo ?? false
  property bool isValidGeometry: mapTool?.isValidGeometry ?? false
  property bool measurementFinalized: mapTool?.measurementFinalized ?? false

  property string perimeter: mapTool?.perimeter ?? 0
  property string area: mapTool?.area ?? 0
  property bool isPolygon: area > 0

  signal measureFinished()

  Component.onCompleted: root.open()

  modal: false
  interactive: false
  closePolicy: Popup.CloseOnEscape

  dropShadow: true

  onClosed: root.measureFinished()

  Behavior on implicitHeight {
    PropertyAnimation { properties: "implicitHeight"; easing.type: Easing.InOutQuad }
  }

  drawerHeader.title: qsTr( "Measure" )

  drawerHeader.topLeftItemContent: MMButton {
    id: btnOne
    text: measurementFinalized ? qsTr( "Repeat" ) : qsTr( "Undo" )
    iconSourceLeft: measurementFinalized ? __style.syncIcon : __style.undoIcon

    type: MMButton.Types.Primary
    size: MMButton.Sizes.Small
    bgndColor: __style.lightGreenColor
    bgndColorDisabled: __style.polarColor
    bgndColorHover: __style.mediumGreenColor
    fontColorHover: __style.forestColor
    iconColorHover: __style.forestColor
    enabled: measurementFinalized || canUndo

    anchors {
      left: parent.left
      leftMargin: __style.pageMargins + __style.safeAreaLeft
      verticalCenter: parent.verticalCenter
    }

    onClicked:
    {
      measurementFinalized ? root.mapTool.resetMeasurement() : root.mapTool.removePoint()
    }
  }

  drawerContent: Column {
    id: mainColumn

    width: parent.width
    spacing: __style.margin10

    Row {
      width: parent.width

      MMGpsComponents.MMGpsDataText{
        width: ( parent.width + parent.spacing ) / 2

        title: measurementFinalized && root.isPolygon ? qsTr( "Perimeter" ) : qsTr( "Length" ) //Perimeter only if its a polygon
        value: __inputUtils.formatDistanceInProjectUnit( root.perimeter, 1, __activeProject.qgsProject )
      }

      MMGpsComponents.MMGpsDataText{
        width: ( parent.width + parent.spacing ) / 2

        title: qsTr( "Area" )
        value: __inputUtils.formatAreaInProjectUnit( root.area, 1, __activeProject.qgsProject )
        alignmentRight: true
        visible: measurementFinalized && root.isPolygon
      }
    }

    Row {
      width: parent.width
      spacing: __style.margin12
      visible: !root.measurementFinalized

      MMButton {
        text: root.canCloseShape ? qsTr( "Close shape" ) : qsTr( "Add point" )
        iconSourceLeft: canCloseShape ? __style.closeShapeIcon : __style.plusIcon
        onClicked: canCloseShape ? root.mapTool.finalizeMeasurement( true ) : root.mapTool.addPoint()
      }

      MMButton {
        type: MMButton.Types.Secondary
        text: qsTr( "Done" )
        iconSourceLeft: __style.doneCircleIcon
        enabled: root.isValidGeometry
        onClicked: root.mapTool.finalizeMeasurement( false )
      }
    }
  }
}
