/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts

Item {
    property string titleText: "title"
    property string descriptionText: "description"
    property bool alignmentRight: false
    property bool itemVisible: true

    width: parent.width / 2
    height: parent.height

    ColumnLayout {
        width: parent.width
        height: parent.height
        visible: itemVisible
        spacing: 0

        Text {
            text: titleText
            color: __style.nightColor
            font: __style.p6
            elide: Text.ElideRight
            horizontalAlignment: alignmentRight ? Text.AlignRight : Text.AlignLeft
            Layout.fillWidth: true
            width: parent.width
            Layout.topMargin: 8
        }

        Text {
            text: descriptionText
            color: __style.nightColor
            font: __style.t3
            elide: Text.ElideMiddle
            horizontalAlignment: alignmentRight ? Text.AlignRight : Text.AlignLeft
            Layout.fillWidth: true
            width: parent.width
            Layout.bottomMargin: 8
        }
    }
}
