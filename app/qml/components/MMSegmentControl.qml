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

Item {
  id: root

  enum Options { All, True, False }

  property int selectedIndex: MMSegmentControl.Options.All

  implicitHeight: __style.row50
  implicitWidth: 3 * ( __style.row50 + 2 * __style.margin20 ) + 2 * __style.margin12

  Rectangle {
    anchors.fill: parent
    radius: __style.radius12
    color: __style.polarColor
  }

  Row {
    anchors.centerIn: parent
    width: parent.width - 2 * __style.margin12
    height: parent.height - 2 * __style.margin8

    Repeater {
      model: 3

      delegate: Item {
        id: segment

        required property int index

        readonly property bool isSelected: root.enabled && root.selectedIndex === index
        readonly property bool isAllOption: index === MMSegmentControl.Options.All

        width: parent.width / 3
        height: parent.height

        // button background
        Rectangle {
          anchors.fill: segment
          radius: __style.radius8

          color: segment.isSelected ? ( segment.isAllOption ? __style.mediumGreenColor : __style.positiveColor ) : __style.transparentColor

          border.color: ( segment.isSelected && !segment.isAllOption ) ? __style.forestColor : __style.transparentColor
          border.width: ( segment.isSelected && !segment.isAllOption ) ? 1.0 * __dp : 0

          MMText {
            anchors.centerIn: parent

            text: {
              switch ( segment.index ) {
                // 0 for All
                case MMSegmentControl.Options.All: return qsTr( "All" )
                // 1 for True
                case MMSegmentControl.Options.True: return qsTr( "True" )
                // 2 for False
                case MMSegmentControl.Options.False: return qsTr( "False" )
              }
              return ""
            }
            font: {
              // bold only if selected
              if ( segment.isSelected ) return __style.t3
              return __style.p5
            }
            color: {
              if ( !root.enabled ) return __style.mediumGreyColor
              if ( segment.isSelected ) return __style.forestColor
              return __style.nightColor
            }
          }
        }

        MouseArea {
          anchors.fill: segment
          enabled: root.enabled
          onClicked: {
            if ( root.selectedIndex !== segment.index ) {
              root.selectedIndex = segment.index
            }
          }
        }
      }
    }
  }
}
