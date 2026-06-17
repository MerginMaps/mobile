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

//
// Header delegate for filter drawers that adds a "No value" option,
// letting users filter features where the field has no value set.
//

MMComponents.MMListDelegate {
  id: root

  required property var currentValue
  required property bool isMultiSelect

  signal addOrRemoveRequested()
  signal selectionRequested()

  property bool checked: root.currentValue ? root.currentValue.some( v => v === null ) : false

  text: qsTr( "No value" )

  rightContent: MMComponents.MMIcon {
    source: __style.doneCircleIcon
    visible: root.checked
  }

  onClicked: {
    if ( root.isMultiSelect ) {
      root.checked = !root.checked
      root.addOrRemoveRequested()
    }
    else {
      root.selectionRequested()
    }
  }
}
