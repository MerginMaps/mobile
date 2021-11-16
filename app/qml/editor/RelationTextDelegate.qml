/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.14
import QtQuick.Controls 2.14
import lc 1.0
import "../"

Item {
  id: root
  property alias backgroundContent: textDelegateContent
  property real firstLinesMaxWidth
  property bool isVisible: {
    if (itemsLine === 0)
      return true;
    if (itemsLine === 1) {
      // this is last line, we want to make sure that I can fit to line with "Add" icon and "More" icon
      if (x + width <= lastLineMaxWidth)
        return true;
    }
    root.setInvisible();
    return false;
  }
  property int itemsLine: {
    // figure out which line am I from Y
    if (y < 2 * height)
      return 0; // first and second line
    if (y < 3 * height)
      return 1; // last line
    return -1; // after last line ~> invisible
  }
  property real lastLineMaxWidth
  property string text
  property alias textContent: txt

  height: customStyle.relationComponent.textDelegateHeight
  visible: isVisible
  width: childrenRect.width

  signal clicked(var feature)
  signal setInvisible

  Rectangle {
    id: textDelegateContent
    property real requestedWidth: txt.paintedWidth + customStyle.relationComponent.tagInnerSpacing

    border.color: customStyle.relationComponent.tagBorderColor
    border.width: customStyle.relationComponent.tagBorderWidth
    color: customStyle.relationComponent.tagBackgroundColor
    height: parent.height
    radius: customStyle.relationComponent.tagRadius
    width: {
      if (root.itemsLine === 0)
        var comparedWidth = root.firstLinesMaxWidth;
      else
        comparedWidth = root.lastLineMaxWidth;
      return requestedWidth > comparedWidth ? comparedWidth : requestedWidth;
    }

    Text {
      id: txt
      clip: true
      color: customStyle.relationComponent.tagTextColor
      font.bold: true
      font.pointSize: customStyle.fields.fontPointSize
      height: parent.height
      horizontalAlignment: Qt.AlignHCenter
      text: root.text ? root.text : model.FeatureTitle
      verticalAlignment: Qt.AlignVCenter
      width: parent.width
    }
  }
  MouseArea {
    anchors.fill: parent

    onClicked: root.clicked(model.FeaturePair)
  }
}
