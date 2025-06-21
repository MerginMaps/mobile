/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

MMDrawer {
  id: root

  property alias title: mmmessage.title
  property alias description: mmmessage.description
  property alias imageSource: mmmessage.image
  property alias link: mmmessage.link
  property alias linkText: mmmessage.linkText

  property alias additionalContent: additionalContentGroup.children

  property alias primaryButton: primaryBtn
  property alias secondaryButton: secondaryBtn

  property real spaceBeforeAdditionalContent: __style.margin20

  signal primaryButtonClicked
  signal secondaryButtonClicked

  dim: true
  interactive: false

  drawerContent: MMScrollView {
    width: parent.width
    height: root.maxHeightHit ? root.drawerContentAvailableHeight : contentHeight

    Column {
      id: col

      width: parent.width

      spacing: 0

      MMMessage {
        id: mmmessage

        width: parent.width
      }

      MMListSpacer { height: root.spaceBeforeAdditionalContent; visible: internal.hasAdditionalContent }

      Item {
        id: additionalContentGroup

        width: parent.width
        // TODO: fix this binding loop
        height: childrenRect.height

        visible: children.length > 0
      }

      MMListSpacer { height: __style.margin40; visible: primaryBtn.visible || secondaryBtn.visible }

      MMButton {
        id: primaryBtn

        width: parent.width
        visible: text.length > 0

        onClicked: primaryButtonClicked()
      }

      MMListSpacer { height: __style.margin20; visible: primaryBtn.visible && secondaryBtn.visible }

      MMButton {
        id: secondaryBtn

        width: parent.width
        visible: text.length > 0

        type: MMButton.Types.Tertiary

        topPadding: 0

        onClicked: secondaryButtonClicked()
      }
    }
  }

  QtObject {
    id: internal

    property bool hasAdditionalContent: additionalContentGroup.children.length > 0
  }
}
