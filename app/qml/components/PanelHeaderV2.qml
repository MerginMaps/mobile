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
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

import ".."

Rectangle {
  id: root

  property string headerTitle
  property string tooltipText

  property bool haveBackButton: true
  property bool haveAccountButton: false

  signal backClicked()
  signal accountClicked()

  function openTooltip() {
    tooltip.open()
  }

  height: InputStyle.rowHeightHeader
  // set width manually

  color: InputStyle.clrPanelMain

  RowLayout {

    spacing: InputStyle.panelSpacing

    anchors {
      fill: parent
      leftMargin: InputStyle.panelMargin
      rightMargin: InputStyle.panelMargin
    }

    Item {
      id: backIcon

      Layout.preferredHeight: InputStyle.iconSizeLarge
      Layout.preferredWidth: InputStyle.iconSizeLarge

      visible: root.haveBackButton

      Image {
        anchors {
          left: parent.left
          verticalCenter: parent.verticalCenter
        }

        source: InputStyle.backIconV2
        sourceSize.width: InputStyle.closeBtnSize
        sourceSize.height: InputStyle.closeBtnSize

        MouseArea {
          anchors {
            fill: parent // make the click area bigger
            leftMargin: -InputStyle.buttonClickArea
            topMargin: -InputStyle.buttonClickArea
            rightMargin: -InputStyle.buttonClickArea
            bottomMargin: -InputStyle.buttonClickArea
          }

          onClicked: root.backClicked()
        }
      }
    }

    Item {
      id: backIconSpacer

      Layout.preferredHeight: parent.height
      Layout.preferredWidth: {
        if ( !root.haveBackButton && !root.haveAccountButton ) {
          // no avatar and no back button
          return 0;
        }
        else if ( root.haveBackButton ) {
          // no account button
          return 0;
        }
        else if ( root.haveAccountButton ) {
          // no back button
          return avatar.width;
        }
        // both visible
        let diff = avatar.width - backIcon.width;
        return Math.max( diff, 0 );
      }
    }

    Text {
      id: title

      Layout.fillWidth: true
      Layout.preferredHeight: parent.height

      text: root.headerTitle

      elide: Text.ElideMiddle
      font.bold: true
      color: InputStyle.fontColor
      font.pixelSize: InputStyle.fontPixelSizeBig

      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter
    }

    Item {
      id: avatarSpacer

      Layout.preferredHeight: parent.height
      Layout.preferredWidth: {
        if ( !root.haveBackButton && !root.haveAccountButton ) {
          // no avatar and no back button
          return 0;
        }
        else if ( root.haveAccountButton ) {
          // no back button
          return 0;
        }
        else if ( root.haveBackButton ) {
          // no avatar
          return backIcon.width;
        }
        // both visible
        let diff = backIcon.width - avatar.width;
        return Math.max( diff, 0 );
      }
    }

    // account head
    Item {
      id: avatar

      Layout.preferredHeight: InputStyle.iconSizeXLarge
      Layout.preferredWidth: InputStyle.iconSizeXLarge

      visible: root.haveAccountButton

      Rectangle {
        id: avatarImage

        anchors.centerIn: parent

        width: avatar.width
        height: avatar.width

        radius: InputStyle.circleRadius
        color: InputStyle.fontColor

        antialiasing: true

        MouseArea {
          anchors {
            fill: parent
            leftMargin: -InputStyle.buttonClickArea
            topMargin: -InputStyle.buttonClickArea
            rightMargin: -InputStyle.buttonClickArea
            bottomMargin: -InputStyle.buttonClickArea
          }

          onClicked: root.accountClicked()
        }

        Image {
          id: userIcon

          anchors.centerIn: avatarImage
          source: InputStyle.accountIcon
          height: avatarImage.height * 0.8
          width: height
          sourceSize.width: width
          sourceSize.height: height
          fillMode: Image.PreserveAspectFit
        }

        ColorOverlay {
          anchors.fill: userIcon
          source: userIcon
          color: "#FFFFFF"
        }
      }

      ToolTip {
        id: tooltip

        y: avatarImage.y
        x: title.x + InputStyle.smallGap

        background: Item {

          // arrow
          Rectangle {
            width: height
            height: parent.height / 3

            rotation: 45
            color: InputStyle.panelBackgroundDarker

            x: bgndRect.x + bgndRect.width - width / 2
            y: bgndRect.y + bgndRect.height / 2 - width / 2
          }

          Rectangle {
            id: bgndRect

            width: parent.width
            height: parent.height
            color: InputStyle.panelBackgroundDarker
            radius: InputStyle.cornerRadius
            antialiasing: true
          }
        }

        contentItem: Label {
          text: root.tooltipText

          font.pixelSize: InputStyle.fontPixelSizeSmall
          color: InputStyle.fontColorWhite
        }
      }
    }
  }
}
