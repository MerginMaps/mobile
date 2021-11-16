/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.8
import QtQuick.Templates 2.1 as T

/**
  * The checkbox where the rectangle area is on left of the text.
  * When text contains html links, the link is opened in the external browser
  */
T.CheckBox {
  id: control
  property var baseColor: InputStyle.panelBackgroundDarker
  property var baseSize: height / 5.5
  property var spaceWidth: height / 4.0

  implicitHeight: Math.max(background ? background.implicitHeight : 0, Math.max(contentItem.implicitHeight, indicator ? indicator.implicitHeight : 0) + topPadding + bottomPadding)
  implicitWidth: Math.max(background ? background.implicitWidth : 0, contentItem.implicitWidth + leftPadding + rightPadding)
  leftPadding: spaceWidth

  signal checkboxClicked(var buttonState)

  onClicked: control.checkboxClicked(control.checkState)

  contentItem: TextHyperlink {
    color: baseColor
    elide: Text.ElideRight
    horizontalAlignment: Text.AlignLeft
    leftPadding: control.indicator.width + 2 * spaceWidth
    text: control.text
  }
  indicator: Rectangle {
    id: checkboxHandle
    anchors.verticalCenter: parent.verticalCenter
    border.color: baseColor
    implicitHeight: baseSize * 2.6
    implicitWidth: baseSize * 2.6
    radius: 2
    x: control.leftPadding

    Rectangle {
      id: rectangle
      color: baseColor
      height: baseSize * 1.4
      radius: baseSize * 0.4
      visible: false
      width: baseSize * 1.4
      x: baseSize * 0.6
      y: baseSize * 0.6
    }

    states: [
      State {
        name: "unchecked"
        when: !control.checked && !control.down
      },
      State {
        name: "checked"
        when: control.checked && !control.down

        PropertyChanges {
          target: rectangle
          visible: true
        }
      },
      State {
        name: "unchecked_down"
        when: !control.checked && control.down

        PropertyChanges {
          color: baseColor
          target: rectangle
        }
        PropertyChanges {
          border.color: baseColor
          target: checkboxHandle
        }
      },
      State {
        extend: "unchecked_down"
        name: "checked_down"
        when: control.checked && control.down

        PropertyChanges {
          target: rectangle
          visible: true
        }
      }
    ]
  }
}
