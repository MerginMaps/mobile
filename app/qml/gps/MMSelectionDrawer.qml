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
import "../components" as MMComponents
import "../map/components"
import "./components" as MMGpsComponents

MMComponents.MMDrawer {
  id: root

  property bool formOpened: false

  property var model
  property var layer

  readonly property var selectedCount: root.model?.count ?? 0

  readonly property alias panelHeight: root.height

  signal selectionFinished()
  signal editSelected()
  signal deleteSelected()
  signal selectionChanged()

  Component.onCompleted: root.open()

  modal: false
  interactive: false
  closePolicy: Popup.CloseOnEscape

  dropShadow: true

  onClosed: root.selectionFinished()

  Behavior on implicitHeight {
    PropertyAnimation { properties: "implicitHeight"; easing.type: Easing.InOutQuad }
  }

  drawerHeader.title: qsTr( "Select features" )

  drawerContent: Column {
    id: mainColumn

    width: parent.width
    spacing: __style.margin10

    Column
    {
      width: parent.width
      visible: true
      spacing: 0

      Column
      {
        width: parent.width
        topPadding: __style.margin8
        bottomPadding: __style.margin8
        spacing: 0

        MMComponents.MMText {
          width: parent.width
          text: qsTr( "%n Feature(s) selected", "", root.selectedCount )
          font: __style.t3
          color: __style.nightColor

          elide: Text.ElideRight
          wrapMode: Text.NoWrap
        }

        MMComponents.MMText {
          width: parent.width
          text: qsTr( "on layer %1" ).arg( root.layer?.name )
          font: __style.p6
          color: __style.nightColor

          elide: Text.ElideRight
          wrapMode: Text.NoWrap
        }
      }
    }

    ScrollView {
        width: parent.width
        height: scrollRow.height

        ScrollBar.vertical.policy: ScrollBar.AlwaysOff
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.horizontal.interactive: true

        contentHeight: scrollRow.height
        contentWidth: scrollRow.width > parent.width ? scrollRow.width : availableWidth

        clip: true

        Row {
          id: scrollRow
          spacing: __style.margin12
          visible: root.selectedCount > 0

          MMComponents.MMButton {
            text: qsTr("Edit")
            iconSourceLeft: __style.editIcon
            onClicked: editSelected()
          }

          MMButton {
            text: qsTr("Delete")
            type: MMButton.Types.Secondary
            fontColor: __style.earthColor
            iconColor: __style.earthColor
            bgndColor: __style.earthColor

            fontColorHover: __style.earthColor
            iconColorHover: __style.earthColor
            bgndColorHover: __style.negativeColor

            iconSourceLeft: __style.deleteIcon
            onClicked: deleteSelected()
            }
        }
    }
  }
}
