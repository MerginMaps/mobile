/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../components" as MMComponents

Column {
  id: root

  required property string filterName

  required property var currentValue

  property string customLabelForTrue: ""
  property string customLabelForFalse: ""

  property var customValueForTrue: null // can be string, number, bool
  property var customValueForFalse: null // can be string, number, bool

  width: parent.width
  spacing: __style.margin8

  MMComponents.MMText {
    width: parent.width

    text: root.filterName

    font: __style.p6
    color: __style.nightColor
    visible: text
  }

  MMComponents.MMSegmentControl {
    id: segControl

    width: parent.width
    backgroundColor: __style.lightGreenColor

    trueText: customLabelForTrue ? customLabelForTrue : qsTr( "True" )
    falseText: customLabelForFalse ? customLabelForFalse : qsTr( "False" )

    Component.onCompleted: {
      if ( root.currentValue && root.currentValue.length === 1 )
      {
        if ( root.currentValue[0] === internal.representationForTrue )
        {
          selectedIndex = MMComponents.MMSegmentControl.Options.True
        }
        else if ( root.currentValue[0] === internal.representationForFalse )
        {
          selectedIndex = MMComponents.MMSegmentControl.Options.False
        }
        else
        {
          selectedIndex = MMComponents.MMSegmentControl.Options.All
        }
      }
      else
      {
        selectedIndex = MMComponents.MMSegmentControl.Options.All
      }
    }

    onSelectedIndexChanged: {
      if ( selectedIndex === MMComponents.MMSegmentControl.Options.All )
      {
        root.currentValue = undefined
      }
      else if ( selectedIndex === MMComponents.MMSegmentControl.Options.True )
      {
        root.currentValue = [internal.representationForTrue]
      }
      else if ( selectedIndex === MMComponents.MMSegmentControl.Options.False )
      {
        root.currentValue = [internal.representationForFalse]
      }
    }
  }

  QtObject {
    id: internal

    property var representationForTrue: root.customValueForTrue ? root.customValueForTrue : true
    property var representationForFalse: root.customValueForFalse ? root.customValueForFalse : false
  }
}
