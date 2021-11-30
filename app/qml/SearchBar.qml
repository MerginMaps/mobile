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

Rectangle {
  id: root
  width: parent.width
  height: InputStyle.rowHeightHeader
  color: InputStyle.panelBackgroundLight

  signal searchTextChanged( string text )

  property string text: ""
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
    searchField.focus = false
    if ( searchField.length > 0 )
      searchField.clear()
    searchTextChanged("")
  }

  onVisibleChanged: {
    // On Android, due to a Qt bug, we need to call deactivate again on page close to clear text search and focus
    if ( !visible && __androidUtils.isAndroid )
      root.deactivate()
  }

  Timer {
    id: searchTimer
    interval: emitInterval
    running: false
    onTriggered: {
      root.text = searchField.displayText
      searchTextChanged( root.text )
    }
  }
  
  Item {
    id: row
    width: root.width
    height: root.height
    
    TextField {
      id: searchField
      width: parent.width - iconContainer.width
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
      
      onDisplayTextChanged: {
        if ( root.allowTimer ) {
          if ( searchTimer.running )
            searchTimer.restart()
          else
            searchTimer.start()
        }
        else
        {
          root.text = searchField.displayText
          root.searchTextChanged( searchField.displayText )
        }
      }
    }
    
    Item {
      id: iconContainer
      height: searchField.height
      width: searchField.height + root.panelMargin
      anchors.right: parent.right
      
      Image {
        id: cancelSearchBtn
        source: searchField.displayText ? InputStyle.noIcon : InputStyle.searchIcon
        width: root.iconSize
        height: width
        sourceSize.width: width
        sourceSize.height: height
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
      }
      
      ColorOverlay {
        anchors.fill: cancelSearchBtn
        source: cancelSearchBtn
        color: root.fontColor
      }

      MouseArea {
        anchors.fill: iconContainer
        onClicked: {
          if ( searchField.displayText ) {
            root.deactivate()
          }
        }
      }
    }
  }
  
  Rectangle {
    id: searchFieldBorder
    color: root.fontColor
    y: searchField.height - height * 4
    height: 2 * __dp
    opacity: searchField.focus ? 1 : 0.6
    width: parent.width - root.panelMargin * 2
    anchors.horizontalCenter: parent.horizontalCenter
  }
}
