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

    readonly property int scrollBarWidth: __inputUtils.isMobilePlatform ? 0 : __style.margin10 
    property bool scrollBarPolicy:  root.ScrollBar.vertical.policy


    contentWidth: availableWidth - scrollBarWidth

    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    // To make sure the scroll bar stays visible on desktop, even when not focused:
    // - both 'policy' and 'visible' properties need to be set, with 'policy' set to AlwaysOn.
    // - the use the 'AsNeeded' policy will make the scroll bar hide when not focused or in use,
    // and only appear after the user starts scrolling.
    ScrollBar.vertical.policy: __inputUtils.isMobilePlatform ? ScrollBar.AlwaysOff : ScrollBar.AlwaysOn
    ScrollBar.vertical.visible: contentHeight > availableHeight ? true  : false
    ScrollBar.vertical.opacity: active ? 0.7 : 0.4
    ScrollBar.vertical.width: scrollBarWidth
}
