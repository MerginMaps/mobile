/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../../components" as MMComponents

/*
 * Empty audio field: "Record audio" and "From device" tiles, mirrors MMPhotoAttachment.
 */

Rectangle {
  id: root

  property bool hasRecordCapability: true

  signal recordClicked()
  signal chooseFromDeviceClicked()

  color: __style.polarColor
  radius: __style.radius20

  Row {
    anchors.centerIn: parent

    width: parent.width - 2 * __style.margin20
    height: parent.height - 2 * __style.margin20

    spacing: __style.spacing20

    Repeater {
      model: [
        { action: "record", icon: __style.microphoneIcon, text: qsTr( "Record audio" ), visible: root.hasRecordCapability },
        { action: "device", icon: __style.audioFileIcon, text: qsTr( "From device" ), visible: true }
      ]

      delegate: Rectangle {
        required property var modelData

        width: internal.tileWidth
        height: parent.height

        visible: modelData.visible

        color: __style.lightGreenColor
        radius: root.radius

        Column {
          width: parent.width
          anchors.centerIn: parent
          spacing: __style.spacing10

          MMComponents.MMIcon {
            anchors.horizontalCenter: parent.horizontalCenter
            source: modelData.icon
            color: root.enabled ? __style.forestColor : __style.mediumGreyColor
          }

          MMComponents.MMText {
            width: parent.width

            font: __style.p6
            text: modelData.text

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideMiddle

            visible: root.enabled
          }
        }

        MMComponents.MMSingleClickMouseArea {
          anchors.fill: parent
          onSingleClicked: {
            if ( modelData.action === "record" ) {
              root.recordClicked()
            }
            else {
              root.chooseFromDeviceClicked()
            }
          }
        }
      }
    }
  }

  QtObject {
    id: internal

    property real tileWidth: {
      if ( !root.hasRecordCapability ) {
        return root.width - 2 * __style.margin20
      }
      return ( root.width - 2 * __style.margin20 - __style.spacing20 ) / 2
    }
  }
}
