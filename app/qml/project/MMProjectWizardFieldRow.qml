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
import QtQuick.Dialogs

import "../components"
import "../inputs"

Item {
  id: fieldDelegate

  signal removeClicked(var index)
  property var widgetList: []

  RowLayout {
    id: row
    height: fieldDelegate.height
    width: fieldDelegate.width
    spacing: 13 * __dp
    property real itemSize: (parent.width - imageBtn.width - (2* row.spacing)) / 2

    MMTextInput {
      id: textField
      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.preferredWidth: row.itemSize

      Component.onCompleted: text = AttributeName
      onTextChanged: AttributeName = text
    }

    MMDropdownInput {
      id: comboBox
      height: row.height
      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.preferredWidth: row.itemSize

      text: AttributeName
      dropDownTitle: qsTr("Widget")
      preselectedFeatures: [WidgetType]
      dataModel: widgetList
      valueRole: "WidgetType"
      textRole: "AttributeName"
    }

    MouseArea {
      id: imageBtn
      width: 40 * __dp
      height: width

      Rectangle {
        color: __style.negativeColor
        anchors.fill: parent
        radius: width / 2

        MMIcon {
          source: __style.deleteIcon
          color: __style.grapeColor
          anchors.verticalCenter: parent.verticalCenter
          anchors.horizontalCenter: parent.horizontalCenter
        }
      }

      onClicked: fieldDelegate.removeClicked(index)
    }
  }
}
