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

    signal checkboxClicked( var buttonState )

    property var spaceWidth: height / 4.0
    property var baseSize: height / 5.5
    property var baseColor: InputStyle.panelBackgroundDarker

    implicitWidth: Math.max(background ? background.implicitWidth : 0,
                                         contentItem.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(background ? background.implicitHeight : 0,
                                          Math.max(contentItem.implicitHeight,
                                                   indicator ? indicator.implicitHeight : 0) + topPadding + bottomPadding)

    leftPadding: spaceWidth

    onClicked: control.checkboxClicked( control.checkState )

    indicator: Rectangle {
        id: checkboxHandle
        implicitWidth: baseSize * 2.6
        implicitHeight: baseSize * 2.6
        x: control.leftPadding
        anchors.verticalCenter: parent.verticalCenter
        radius: 2
        border.color: baseColor

        Rectangle {
            id: rectangle
            width: baseSize * 1.4
            height: baseSize * 1.4
            x: baseSize * 0.6
            y: baseSize * 0.6
            radius: baseSize * 0.4
            visible: false
            color: baseColor
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
                    target: rectangle
                    color: baseColor
                }

                PropertyChanges {
                    target: checkboxHandle
                    border.color: baseColor
                }
            },
            State {
                name: "checked_down"
                extend: "unchecked_down"
                when: control.checked && control.down

                PropertyChanges {
                    target: rectangle
                    visible: true
                }
            }
        ]
    }

    contentItem: TextHyperlink {
        leftPadding: control.indicator.width + 2 * spaceWidth
        color: baseColor
        text: control.text
        elide: Text.ElideRight
        horizontalAlignment: Text.AlignLeft
    }
}
