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

ScrollView {
  id: root

  required property list<color> colors

  property color activeColor

  height: scrollRow.height
  ScrollBar.vertical.policy: ScrollBar.AlwaysOff
  ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

  Row {
    id: scrollRow
    spacing: __style.margin2
    anchors.centerIn: parent

    Repeater {
      model: root.colors
      MMColorButton{
        required property color modelData
        required property int index
                
        buttonColor: modelData
        isSelected: root.activeColor === modelData
                
        onClicked: {
          root.activeColor = modelData;
        }
        Component.onCompleted: {
        // set the initial color to be the first one in the list
          if ( index === 0 )
          {
            root.activeColor = modelData
          }    
        }  
      } 
    }
  }
}