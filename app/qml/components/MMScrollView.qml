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
import QtQml

ScrollView {
    id: root

    readonly property bool isMobile: (Qt.platform.os === "android"
                                      || Qt.platform.os === "ios")
    readonly property int scrollBarWidth: !isMobile ? __style.margin10 : 0
    property bool showScrollBar:  root.ScrollBar.vertical.policy


    contentWidth: availableWidth - scrollBarWidth

    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    // vertical scroll bar should appear only when needed
    ScrollBar.vertical.policy: isMobile ? ScrollBar.AlwaysOff : ScrollBar.AlwaysOn
    ScrollBar.vertical.visible: contentHeight > availableHeight ? true  : false
    ScrollBar.vertical.opacity: active ? 0.7 : 0.4
}
