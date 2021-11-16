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
  property bool allowTimer: false
  property color bgColor: InputStyle.panelBackgroundLight
  property int emitInterval: 200
  property color fontColor: InputStyle.panelBackgroundDarker
  property real iconSize: rowHeight / 3
  property real panelMargin: InputStyle.panelMargin
  property real rowHeight: InputStyle.rowHeightHeader * 1.2
  property bool showMergin: false
  property string text: ""

  color: InputStyle.panelBackgroundLight
  height: InputStyle.rowHeightHeader
  width: parent.width

  /**
    * Used for deactivating focus on SearchBar when another component should have focus.
    * and the current element's forceActiveFocus() doesnt deactivates SearchBar focus.
    */
  function deactivate() {
    searchField.focus = false;
    if (searchField.length > 0)
      searchField.clear();
    searchTextChanged("");
  }
  signal searchTextChanged(string text)

  onVisibleChanged: {
    // On Android, due to a Qt bug, we need to call deactivate again on page close to clear text search and focus
    if (!visible && __androidUtils.isAndroid)
      root.deactivate();
  }

  Timer {
    id: searchTimer
    interval: emitInterval
    running: false

    onTriggered: {
      root.text = searchField.displayText;
      searchTextChanged(root.text);
    }
  }
  Item {
    id: row
    height: root.height
    width: root.width

    TextField {
      id: searchField
      color: root.fontColor
      font.capitalization: Font.MixedCase
      font.pixelSize: InputStyle.fontPixelSizeNormal
      height: InputStyle.rowHeight
      inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
      leftPadding: root.panelMargin
      placeholderText: qsTr("SEARCH")
      rightPadding: root.panelMargin
      width: parent.width - iconContainer.width

      onDisplayTextChanged: {
        if (root.allowTimer) {
          if (searchTimer.running)
            searchTimer.restart();
          else
            searchTimer.start();
        } else {
          root.text = searchField.displayText;
          root.searchTextChanged(searchField.displayText);
        }
      }

      background: Rectangle {
        color: root.bgColor
      }
    }
    Item {
      id: iconContainer
      anchors.right: parent.right
      height: searchField.height
      width: searchField.height + root.panelMargin

      Image {
        id: cancelSearchBtn
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        height: width
        source: searchField.displayText ? InputStyle.noIcon : InputStyle.searchIcon
        sourceSize.height: height
        sourceSize.width: width
        width: root.iconSize
      }
      ColorOverlay {
        anchors.fill: cancelSearchBtn
        color: root.fontColor
        source: cancelSearchBtn
      }
      MouseArea {
        anchors.fill: iconContainer

        onClicked: {
          if (searchField.displayText) {
            root.deactivate();
          }
        }
      }
    }
  }
  Rectangle {
    id: searchFieldBorder
    anchors.horizontalCenter: parent.horizontalCenter
    color: root.fontColor
    height: 2 * QgsQuick.Utils.dp
    opacity: searchField.focus ? 1 : 0.6
    width: parent.width - root.panelMargin * 2
    y: searchField.height - height * 4
  }
}
