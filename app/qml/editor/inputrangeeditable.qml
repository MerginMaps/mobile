/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14
import QgsQuick 0.1 as QgsQuick
import lc 1.0

AbstractEditor {
  id: root
  /*required*/
  property bool isReadOnly: parent.readOnly
  property var locale: Qt.locale()

  /*required*/
  property var parentValue: parent.value
  /*required*/
  property bool parentValueIsNull: parent.valueIsNull
  property real precision: config['Precision'] ? config['Precision'] : 0

  // don't ever use a step smaller than would be visible in the widget
  // i.e. if showing 2 decimals, smallest increment will be 0.01
  // https://github.com/qgis/QGIS/blob/a038a79997fb560e797daf3903d94c7d68e25f42/src/gui/editorwidgets/qgsdoublespinbox.cpp#L83-L87
  property real step: Math.max(config["Step"], Math.pow(10.0, 0.0 - precision))
  property string suffix: config['Suffix'] ? config['Suffix'] : ''

  enabled: !isReadOnly

  signal editorValueChanged(var newValue, bool isNull)

  onContentClicked: {
    numberInput.forceActiveFocus();
  }
  onLeftActionClicked: {
    if (minusSign.enabled) {
      let decremented = Number(numberInput.text) - root.step;
      root.editorValueChanged(decremented.toFixed(root.precision), false);
    }
  }
  onRightActionClicked: {
    if (plusSign.enabled) {
      let incremented = Number(numberInput.text) + root.step;
      root.editorValueChanged(incremented.toFixed(root.precision), false);
    }

    // on press and hold behavior can be used from here:
    // https://github.com/mburakov/qt5/blob/93bfa3874c10f6cb5aa376f24363513ba8264117/qtquickcontrols/src/controls/SpinBox.qml#L306-L309
  }

  content: Item {
    id: contentContainer
    anchors.fill: parent

    Row {
      id: inputAndSuffixContainer
      height: parent.height
      width: numberInput.contentWidth + suffix.contentWidth > parent.width ? parent.width : numberInput.contentWidth + suffix.contentWidth
      x: parent.width / 2 - width / 2

      TextInput {
        id: numberInput
        clip: true
        color: customStyle.fields.fontColor
        font.pointSize: customStyle.fields.fontPointSize
        height: parent.height
        horizontalAlignment: Qt.AlignRight
        inputMethodHints: Qt.ImhFormattedNumbersOnly
        selectedTextColor: "#ffffff"
        selectionColor: customStyle.fields.fontColor
        text: root.parentValue === undefined || root.parentValueIsNull ? "" : root.parentValue
        verticalAlignment: Qt.AlignVCenter
        width: contentWidth < (parent.width - suffix.width) ? contentWidth : parent.width - suffix.width

        onTextEdited: {
          let val = text.replace(",", ".").replace(/ /g, ''); // replace comma with dot
          root.editorValueChanged(val, val === "");
        }
      }
      Text {
        id: suffix
        color: customStyle.fields.fontColor
        font.pointSize: customStyle.fields.fontPointSize
        height: parent.height
        horizontalAlignment: Qt.AlignLeft
        text: root.suffix
        verticalAlignment: Qt.AlignVCenter
        visible: root.suffix !== "" && numberInput.text !== ""
        width: paintedWidth
      }
    }
  }
  leftAction: Item {
    id: minusSign
    anchors.fill: parent
    enabled: Number(numberInput.text) - root.step >= config["Min"]

    Image {
      id: imgMinus
      anchors.centerIn: parent
      source: customStyle.icons.minus
      sourceSize.width: parent.width / 3
      width: parent.width / 3
    }
    ColorOverlay {
      anchors.fill: imgMinus
      color: minusSign.enabled ? customStyle.fields.fontColor : customStyle.toolbutton.backgroundColorInvalid
      source: imgMinus
    }
  }
  rightAction: Item {
    id: plusSign
    anchors.fill: parent
    enabled: Number(numberInput.text) + root.step <= config["Max"]

    Image {
      id: imgPlus
      anchors.centerIn: parent
      source: customStyle.icons.plus
      sourceSize.width: parent.width / 3
      width: parent.width / 3
    }
    ColorOverlay {
      anchors.fill: imgPlus
      color: plusSign.enabled ? customStyle.fields.fontColor : customStyle.toolbutton.backgroundColorInvalid
      source: imgPlus
    }
  }
}
