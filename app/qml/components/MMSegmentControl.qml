/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

/**
  * A three-state segment control for boolean-like selection: All / True / False.
  *
  * Signals:
  *   - selectionChanged( int index ) — emitted when the selected segment changes;
  *     index maps to MMSegmentControl.Options enum values.
  *
  * Usage:
  *   MMSegmentControl {
  *     onSelectionChanged: function( index ) { console.log( index ) }
  *   }
  */
Item {
  id: root

  enum Options { All, True, False }

  /* optional */ property int selectedIndex: MMSegmentControl.Options.All

  /* optional */ property string allText: qsTr( "All" )
  /* optional */ property string trueText: qsTr( "True" )
  /* optional */ property string falseText: qsTr( "False" )

  signal selectionChanged( int index )

  implicitWidth: row.implicitWidth + 2 * __style.margin8
  implicitHeight: __style.row45

  Rectangle {
    id: background

    anchors.fill: parent
    radius: __style.radius16
    color: __style.lightGreenColor
  }

  Row {
    id: row

    anchors.fill: parent
    anchors.margins: __style.margin4

    spacing: 0

    Repeater {
      id: repeater

      model: [
        { text: root.allText,   index: MMSegmentControl.Options.All   },
        { text: root.trueText,  index: MMSegmentControl.Options.True  },
        { text: root.falseText, index: MMSegmentControl.Options.False }
      ]

      delegate: Item {
        id: segment

        readonly property bool isSelected: root.selectedIndex === modelData.index

        width: ( row.width - row.spacing * ( repeater.count - 1 ) ) / repeater.count
        height: row.height

        Rectangle {
          id: segmentBackground

          anchors.fill: parent
          radius: __style.radius12

          color: segment.isSelected ? __style.polarColor : __style.transparentColor

          border.color: segment.isSelected ? __style.forestColor : __style.transparentColor
          border.width: segment.isSelected ? 1.5 * __dp : 0
        }

        MMText {
          id: label

          anchors.centerIn: parent

          text: modelData.text
          font: __style.t4
          color: segment.isSelected ? __style.forestColor : __style.nightColor

          fontSizeMode: Text.Fit
          minimumPixelSize: 10 * __dp
          horizontalAlignment: Text.AlignHCenter

          leftPadding: __style.margin8
          rightPadding: __style.margin8
        }

        MouseArea {
          anchors.fill: parent
          onClicked: {
            if ( root.selectedIndex !== modelData.index ) {
              root.selectedIndex = modelData.index
              root.selectionChanged( modelData.index )
            }
          }
        }
      }
    }
  }
}
