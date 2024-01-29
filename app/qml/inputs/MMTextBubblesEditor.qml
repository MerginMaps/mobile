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
import "../components"

MMAbstractEditor {
  id: root

  property var parentValue: parent.value ?? ""
  property bool parentValueIsNull: parent.valueIsNull ?? false
  property bool isReadOnly: parent.readOnly ?? false

  readonly property real itemHeight: 36 * __dp
  readonly property int rows: 3
  required property ListModel featuresModel

  signal editorValueChanged( var newValue, var isNull )

  contentItemHeight: itemHeight * rows + 2 * flow.spacing + 20 * __dp

  Component.onCompleted: root.hideFeaturesIfNeeded()
  onWidthChanged: root.hideFeaturesIfNeeded()

  content: Rectangle {
    width: root.width - 2 * root.spacing
    height: root.contentItemHeight
    color: __style.whiteColor

    Flow{
      id: flow

      anchors.fill: parent
      anchors.margins: 10 * __dp
      spacing: 8 * __dp
      clip: true

      Rectangle {
        width: 40 * __dp
        height: root.itemHeight
        radius: 8 * __dp
        color: __style.lightGreenColor

        MMIcon {
          anchors.centerIn: parent
          source: __style.plusIcon
          useCustomSize: true
          width: 16 * __dp
          height: width
        }
      }

      Repeater {
        id: repeater

        property var invisibleIds: []

        model: root.featuresModel

        delegate: Rectangle {
          width: text.contentWidth + 24 * __dp
          height: root.itemHeight
          radius: 8 * __dp
          color: __style.mediumGreenColor
          visible: {
            let isVisible = (y < 2 * root.itemHeight || (y < 3 * root.itemHeight && x + width < flow.width - footer.width - flow.spacing))
            if(!isVisible && !repeater.invisibleIds.includes(model.FeatureId))
              repeater.invisibleIds.push(model.FeatureId)
            footer.visible = repeater.invisibleIds.length > 0
            moreText.text = ("+" + repeater.invisibleIds.length + qsTr(" more"))
            return isVisible
          }

          Text {
            id: text

            anchors.centerIn: parent
            font: __style.t4
            text: model.FeatureTitle + model.Description
            color: __style.forestColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
          }
        }
      }

      Rectangle {
        id: footer

        width: 100 * __dp
        height: root.itemHeight
        radius: 8 * __dp
        color: __style.lightGreenColor

        Text {
          id: moreText

          anchors.centerIn: parent
          font: __style.t4
          color: __style.forestColor
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
        }
      }
    }
  }

  function hideFeaturesIfNeeded() {
    repeater.invisibleIds = []
    repeater.model = undefined
    repeater.model = root.featuresModel
  }
}
