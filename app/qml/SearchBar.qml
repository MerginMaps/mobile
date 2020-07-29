/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick

Rectangle {
  id: root
  width: parent.width
  height: InputStyle.rowHeightHeader
  color: InputStyle.panelBackgroundLight

  signal searchTextChanged( string text )

  property string text: searchField.text
  property bool allowTimer: false
  property int emitInterval: 200
  
  property color bgColor: InputStyle.panelBackgroundLight
  property color fontColor: InputStyle.panelBackgroundDarker

  property real rowHeight: InputStyle.rowHeightHeader * 1.2
  property real iconSize: rowHeight/3
  property bool showMergin: false
  property real panelMargin: InputStyle.panelMargin

  /**
    * Used for deactivating focus on SearchBar when another component should have focus.
    * and the current element's forceActiveFocus() doesnt deactivates SearchBar focus.
    */
  function deactivate() {
    searchField.text = ""
    searchField.focus = false
  }

  Timer {
    id: searchTimer
    interval: emitInterval
    running: false
    onTriggered: {
      root.text = searchField.text
      searchTextChanged( root.text )
    }
  }
  
  Item {
    id: row
    width: root.width
    height: root.height
    
    TextField {
      id: searchField
      width: parent.width
      height: InputStyle.rowHeight
      font.pixelSize: InputStyle.fontPixelSizeNormal
      color: root.fontColor
      placeholderText: qsTr("SEARCH")
      font.capitalization: Font.MixedCase
      inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
      background: Rectangle {
        color: root.bgColor
      }

      leftPadding: root.panelMargin
      rightPadding: root.panelMargin
      
      onTextChanged: {
        if ( root.allowTimer ) {
          if ( searchTimer.running )
            searchTimer.restart()
          else
            searchTimer.start()
        }
        else
        {
          root.text = searchField.text
          root.searchTextChanged( text )
        }
      }
    }
    
    Item {
      id: iconContainer
      height: searchField.height
      width: root.iconSize
      anchors.right: parent.right
      anchors.rightMargin: root.panelMargin
      
      Image {
        id: cancelSearchBtn
        source: searchField.text ? "no.svg" : "search.svg"
        width: root.iconSize
        height: width
        sourceSize.width: width
        sourceSize.height: height
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        
        MouseArea {
          anchors.fill: parent
          onClicked: {
            if (searchField.text) {
              root.deactivate()
            }
          }
        }
      }
      
      ColorOverlay {
        anchors.fill: cancelSearchBtn
        source: cancelSearchBtn
        color: root.fontColor
      }
    }
  }
  
  Rectangle {
    id: searchFieldBorder
    color: root.fontColor
    y: searchField.height - height * 4
    height: 2 * QgsQuick.Utils.dp
    opacity: searchField.focus ? 1 : 0.6
    width: parent.width - root.panelMargin * 2
    anchors.horizontalCenter: parent.horizontalCenter
  }
}
