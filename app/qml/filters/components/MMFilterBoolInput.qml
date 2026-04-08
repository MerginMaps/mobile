/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../components"

Column {
  id: root

  width: parent.width
  spacing: __style.margin8

  required property string fieldDisplayName
  required property var currentValue
  required property string fieldLayerId
  required property string fieldName

  property bool _initialized: false
  Component.onCompleted: _initialized = true

  MMText {
    width: parent.width
    text: root.fieldDisplayName
    font: __style.p6
    color: __style.nightColor
    visible: root.fieldDisplayName !== ""
  }

  MMSegmentControl {
    id: segControl

    width: parent.width
    backgroundColor: __style.lightGreenColor

    selectedIndex: {
      let val = root.currentValue
      if ( val === null || val === undefined ) return MMSegmentControl.Options.All
      return ( val === true || val === 1 ) ? MMSegmentControl.Options.True : MMSegmentControl.Options.False
    }

    onSelectedIndexChanged: {
      if ( !root._initialized || !root.fieldLayerId || !root.fieldName ) return
      switch ( segControl.selectedIndex ) {
        case MMSegmentControl.Options.All:
          __activeProject.filterController.removeFieldFilter( root.fieldLayerId, root.fieldName )
          break
        case MMSegmentControl.Options.True:
          __activeProject.filterController.setFieldFilter( root.fieldLayerId, root.fieldName, "bool", true )
          break
        case MMSegmentControl.Options.False:
          __activeProject.filterController.setFieldFilter( root.fieldLayerId, root.fieldName, "bool", false )
          break
      }
    }
  }
}
