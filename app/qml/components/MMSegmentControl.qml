/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

/**
  * Three-state segment control: All / True / False.
  * Emits selectionChanged( int index ) on tap; index maps to MMSegmentControl.Options.
  */
Item {
  id: root

  enum Options { All, True, False }

  /* optional */ property int selectedIndex: MMSegmentControl.Options.All

  /* optional */ property string allText: qsTr( "All" )
  /* optional */ property string trueText: qsTr( "True" )
  /* optional */ property string falseText: qsTr( "False" )

  signal selectionChanged( int index )

  implicitHeight: __style.row50
  implicitWidth: {
    let maxW = Math.max( allMeasure.implicitWidth, trueMeasure.implicitWidth, falseMeasure.implicitWidth )
    return 3 * ( maxW + 2 * __style.margin20 ) + 2 * __style.margin13
  }

  MMText { id: allMeasure;   text: root.allText;   font: __style.t4; visible: false }
  MMText { id: trueMeasure;  text: root.trueText;  font: __style.t4; visible: false }
  MMText { id: falseMeasure; text: root.falseText; font: __style.t4; visible: false }

  Rectangle {
    anchors.fill: parent
    radius: __style.radius12
    color: __style.primaryColor
  }

  Row {
    anchors.centerIn: parent
    width: parent.width - 2 * __style.margin13
    height: parent.height - 2 * __style.margin8

    spacing: 0

    Repeater {
      model: [
        { text: root.allText,   index: MMSegmentControl.Options.All   },
        { text: root.trueText,  index: MMSegmentControl.Options.True  },
        { text: root.falseText, index: MMSegmentControl.Options.False }
      ]

      delegate: Item {
        id: segment

        required property var modelData

        readonly property bool isSelected: root.enabled && root.selectedIndex === segment.modelData.index
        readonly property bool isAllOption: segment.modelData.index === MMSegmentControl.Options.All

        width: parent.width / 3
        height: parent.height

        Rectangle {
          anchors.fill: parent
          radius: __style.radius8

          visible: segment.isSelected

          color: segment.isAllOption ? __style.mediumGreenColor : __style.positiveColor

          border.color: segment.isAllOption ? __style.transparentColor : __style.forestColor
          border.width: segment.isAllOption ? 0 : 1.5 * __dp
        }

        MMText {
          anchors.centerIn: parent

          text: segment.modelData.text
          font: __style.t4
          color: {
            if ( !root.enabled ) return __style.darkGreenColor
            if ( segment.isSelected ) return __style.forestColor
            return __style.nightColor
          }

          horizontalAlignment: Text.AlignHCenter

          leftPadding: __style.margin8
          rightPadding: __style.margin8
        }

        MouseArea {
          anchors.fill: parent
          enabled: root.enabled
          onClicked: {
            if ( root.selectedIndex !== segment.modelData.index ) {
              root.selectedIndex = segment.modelData.index
              root.selectionChanged( segment.modelData.index )
            }
          }
        }
      }
    }
  }
}
