/***************************************************************************
 checkbox.qml
  --------------------------------------
  Date                 : 2017
  Copyright            : (C) 2017 by Matthias Kuhn
  Email                : matthias@opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Controls 2.2
import "../components"

/**
 * Checkbox for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section
 * Do not use directly from Application QML
 */
Item {
  id: fieldItem
  property string booleanEnum: "1" // QMetaType::Bool Enum of Qvariant::Type
  property var checkedState: getConfigValue(config['CheckedState'], true)
  property bool isReadOnly: readOnly
  property var uncheckedState: getConfigValue(config['UncheckedState'], false)

  enabled: !readOnly
  height: childrenRect.height

  signal editorValueChanged(var newValue, bool isNull)
  function getConfigValue(configValue, defaultValue) {
    if (!configValue && field.type + "" === fieldItem.booleanEnum) {
      return defaultValue;
    } else
      return configValue;
  }

  anchors {
    left: parent.left
    right: parent.right
  }
  Rectangle {
    id: fieldContainer
    color: customStyle.fields.backgroundColor
    height: customStyle.fields.height
    radius: customStyle.fields.cornerRadius

    anchors {
      left: parent.left
      right: parent.right
    }
    MouseArea {
      anchors.fill: parent

      onClicked: switchComp.toggle()
    }
    Text {
      anchors.left: parent.left
      anchors.verticalCenter: parent.verticalCenter
      color: customStyle.fields.fontColor
      font.pointSize: customStyle.fields.fontPointSize
      horizontalAlignment: Text.AlignLeft
      leftPadding: customStyle.fields.sideMargin
      text: switchComp.checked ? fieldItem.checkedState : fieldItem.uncheckedState
      verticalAlignment: Text.AlignVCenter
    }
    Switch {
      id: switchComp
      property var currentValue: value

      anchors.right: parent.right
      anchors.rightMargin: customStyle.fields.sideMargin
      anchors.verticalCenter: parent.verticalCenter
      bgndColorActive: customStyle.toolbutton.activeButtonColor
      bgndColorInactive: customStyle.toolbutton.backgroundColorInvalid
      checked: value === fieldItem.checkedState
      implicitHeight: fieldContainer.height * 0.6
      isReadOnly: fieldItem.isReadOnly

      // Workaround to get a signal when the value has changed
      onCurrentValueChanged: {
        switchComp.checked = currentValue === fieldItem.checkedState;
      }
      onSwitchChecked: {
        editorValueChanged(isChecked ? fieldItem.checkedState : fieldItem.uncheckedState, false);
      }
    }
  }
}
