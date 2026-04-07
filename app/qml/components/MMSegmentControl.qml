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
  property color backgroundColor: __style.polarColor

  property string allText: qsTr( "All" )
  property string trueText: qsTr( "True" )
  property string falseText: qsTr( "False" )

  implicitHeight: __style.row50
  implicitWidth: 3 * ( __style.row50 + 2 * __style.margin20 ) + 2 * __style.margin12

  Rectangle {
    anchors.fill: parent
    radius: __style.radius12
    color: root.backgroundColor
  }

  Row {
    anchors.fill: parent
    leftPadding: __style.margin12
    rightPadding: __style.margin12
    topPadding: __style.margin8
    bottomPadding: __style.margin8

    Repeater {
      model: 3

      delegate: Rectangle {
        id: segment

        required property int index

        readonly property bool isSelected: root.enabled && root.selectedIndex === index
        readonly property bool isAllOption: index === MMSegmentControl.Options.All

        width: ( parent.width - parent.leftPadding - parent.rightPadding ) / 3
        height: parent.height - parent.topPadding - parent.bottomPadding

        radius: __style.radius8

        color: isSelected ? ( isAllOption ? __style.mediumGreenColor : __style.positiveColor ) : __style.transparentColor

        border.color: ( isSelected && !isAllOption ) ? __style.forestColor : __style.transparentColor
        border.width: ( isSelected && !isAllOption ) ? 1.0 * __dp : 0

        MMText {
          anchors.centerIn: parent
          // extra padding
          width: parent.width - 2 * __style.margin8
          horizontalAlignment: Text.AlignHCenter

          text: {
            switch ( segment.index ) {
              case MMSegmentControl.Options.All:   return root.allText
              case MMSegmentControl.Options.True:  return root.trueText
              case MMSegmentControl.Options.False: return root.falseText
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

        MouseArea {
          anchors.fill: parent
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
