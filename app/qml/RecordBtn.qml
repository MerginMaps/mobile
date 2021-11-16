/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QgsQuick 0.1 as QgsQuick

Item {
  id: recordBtnContainer
  property int border: 10 * QgsQuick.Utils.dp
  property color color: InputStyle.clrPanelMain
  property bool recording: false
  property int size: width / 2

  height: width

  function activated() {
    animation.start();
  }

  onRecordingChanged: {
    if (recording === false) {
      recBtn.borderWidth = recordBtnContainer.border;
    }
  }

  Rectangle {
    id: recBtn
    property int borderWidth: recordBtnContainer.border

    anchors.centerIn: parent
    antialiasing: true
    border.color: recordBtnContainer.color
    border.width: borderWidth
    color: recording ? "#fd5757" : "transparent"
    height: size
    radius: width * 0.5
    width: size

    SequentialAnimation {
      id: animation
      loops: Animation.Infinite
      running: recording

      NumberAnimation {
        duration: 300
        from: recordBtnContainer.border
        property: "borderWidth"
        target: recBtn
        to: recordBtnContainer.border * 0.7
      }
      NumberAnimation {
        duration: 300
        from: recordBtnContainer.border * 0.7
        property: "borderWidth"
        target: recBtn
        to: recordBtnContainer.border
      }
    }
  }
}
