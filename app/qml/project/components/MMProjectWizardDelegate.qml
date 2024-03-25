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

import "../../components"
import "../../inputs"

Item {
  id: root

  signal removeClicked(var index)
  property var widgetList
  property var widgetType2WidgetName

  RowLayout {
    id: row
    height: root.height
    width: root.width
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

    // TODO: replace with MMComboboxInput

//    MMDropdownInput {
//      id: comboBox
//      height: row.height
//      Layout.fillHeight: true
//      Layout.fillWidth: true
//      Layout.preferredWidth: row.itemSize

//      text: root.widgetType2WidgetName[WidgetType]

//      dropDownTitle: qsTr("Widget")
//      preselectedFeatures: [WidgetType]
//      dataModel: widgetList
//      valueRole: "WidgetType"
//      textRole: "WidgetName"

//      onSelectionFinished: function( newValue ) {
//        WidgetType = newValue[0]
//      }
//    }

    MMRoundButton {
      id: imageBtn

      iconSource: __style.deleteIcon
      iconColor: __style.grapeColor
      bgndColor: __style.negativeColor
      bgndHoverColor: __style.negativeColor

      onClicked: root.removeClicked(index)
    }
  }
}
