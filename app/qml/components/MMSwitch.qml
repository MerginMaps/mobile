/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Switch {
  id: control

  property string textOn
  property string textOff
  property bool visibleText: true

  contentItem: Text {
    text: {
      if(control.visibleText) {
        if(control.textOn.length > 0 && control.textOff.length > 0) {
          if(control.checked)
            return control.textOn
          return control.textOff
        }
        return control.text
      }
    }
    font: __style.p5
    color: control.enabled ? __style.forestColor : __style.mediumGreenColor
    verticalAlignment: Text.AlignVCenter
    leftPadding: control.indicator.width + control.spacing
  }

  indicator: Rectangle {
    implicitWidth: 48
    implicitHeight: 28
    x: control.leftPadding
    y: parent.height / 2 - height / 2
    radius: implicitHeight / 2
    color: control.checked ? __style.grassColor : __style.whiteColor

    Rectangle {
      x: control.checked ? parent.width - width - radius/2 : radius/2
      width: 20
      height: width
      radius: width / 2
      color: control.enabled ? __style.forestColor : __style.mediumGreenColor
      anchors.verticalCenter: parent.verticalCenter
    }
  }
}
