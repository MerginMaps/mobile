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

Item {
  id: fieldItem

  signal valueChanged( var value, bool isNull )

  property var locale: Qt.locale()
  property real parentValue: fieldItem.parent.value ? fieldItem.parent.value : 0

  function toNumber( localeString ) {
    try {
      var d = Number.fromLocaleString( locale, localeString )
    }
    catch ( error ) {
      d = Number( localeString )
    }

    return d
  }

  RangeWidgetHelper {
    id: helper

    widgetConfig: config
  }

  enabled: !readOnly
  height: customStyle.fields.height

  anchors {
    left: parent.left
    right: parent.right
  }

  // background
  Rectangle {
    anchors.fill: parent
    border.color: customStyle.fields.normalColor
    border.width: 1 * QgsQuick.Utils.dp
    color: customStyle.fields.backgroundColor
    radius: customStyle.fields.cornerRadius
  }

  Item {
    id: contentContainer

    anchors.fill: parent

    RowLayout {

      anchors.fill: parent

      Item {
        id: minusSign

        enabled: toNumber( numberInput.displayText ) - helper.step >= config["Min"]

        Layout.preferredHeight: parent.height
        Layout.maximumHeight: parent.height
        Layout.preferredWidth: parent.height
        Layout.alignment: Qt.AlignLeft | Qt.AlignTop

        Image {
          id: imgMinus

          anchors.centerIn: parent

          width: parent.width / 3
          sourceSize.width: parent.width / 3

          source: customStyle.icons.minus
        }

        ColorOverlay {
          source: imgMinus
          color: minusSign.enabled ? customStyle.fields.fontColor : customStyle.toolbutton.backgroundColorInvalid
          anchors.fill: imgMinus
        }

        MouseArea {
          width: parent.width * 1.3
          height: parent.width * 1.3
          x: -(0.1 * width)
          y: -(0.1 * height)

          onClicked: {
            let v = toNumber( numberInput.displayText )

            v -= helper.step
            valueChanged( v.toFixed( helper.precision ), false )
          }
        }
      }

      Item {
        id: valueAndSuffixContainer

        Layout.preferredHeight: parent.height
        Layout.maximumHeight: parent.height
        Layout.fillWidth: true

        RowLayout {

          anchors.fill: parent

          Item {
            id: valueInputContainer

            Layout.preferredHeight: parent.height
            Layout.maximumHeight: parent.height

            Layout.minimumWidth: parent.width / 2
            Layout.maximumWidth: parent.width - suffixContainer.width
            Layout.preferredWidth: parent.width / 2 + numberInput.contentWidth * 0.38

            TextInput {
              id: numberInput

              onTextEdited: {
                console.log( "Text edited to: ", text )
//                valueChanged( text, text === "" )
              }

              onEditingFinished: {
                console.log( "Text editing finished with text: ", text )
              }

              anchors.fill: parent

              validator: helper.validator
              text: {
                console.log("is composing? ", inputMethodComposing)
                fieldItem.parent.value ? Number( fieldItem.parent.value ).toLocaleString( locale, 'f', helper.precision ) : ""
              }

              inputMethodHints: Qt.ImhFormattedNumbersOnly
              font.pointSize: customStyle.fields.fontPointSize
              color: customStyle.fields.fontColor
              selectionColor: customStyle.fields.fontColor
              selectedTextColor: "#ffffff"

              horizontalAlignment: Qt.AlignRight
              verticalAlignment: Qt.AlignVCenter

              clip: true
            }

//            Rectangle{
//              anchors.fill: parent
//              color: "blue"
//              opacity: .3
//            }
          }

          Item {
            id: suffixContainer

            property bool shouldShowSuffix: helper.suffix !== "" && numberInput.text !== ""

            Layout.preferredHeight: parent.height
            Layout.maximumHeight: parent.height
            Layout.fillWidth: true
            Layout.minimumWidth: suffix.paintedWidth
            visible: shouldShowSuffix

            Text {
              id: suffix

              text: helper.suffix

              anchors.fill: parent
              horizontalAlignment: Qt.AlignLeft
              verticalAlignment: Qt.AlignVCenter

              color: customStyle.fields.fontColor
              font.pointSize: customStyle.fields.fontPointSize
            }

            MouseArea {
              anchors.fill: parent

              // pass focus to number editor, suffix is not editable
              onClicked: numberInput.forceActiveFocus()
            }
          }
        }
      }

      Item {
        id: plusSign

        enabled: toNumber( numberInput.displayText ) + helper.step <= config["Max"]

        Layout.preferredHeight: parent.height
        Layout.maximumHeight: parent.height
        Layout.preferredWidth: parent.height
        Layout.alignment: Qt.AlignRight | Qt.AlignTop

        Image {
          id: imgPlus

          anchors.centerIn: parent

          width: parent.width / 3
          sourceSize.width: parent.width / 3

          source: customStyle.icons.plus
        }

        ColorOverlay {
          source: imgPlus
          color: plusSign.enabled ? customStyle.fields.fontColor : customStyle.toolbutton.backgroundColorInvalid
          anchors.fill: imgPlus
        }

        MouseArea {
          width: parent.width * 1.3
          height: parent.width * 1.3
          x: -(0.1 * width)
          y: -(0.1 * height)

          onClicked: {
            let v = toNumber( numberInput.displayText )

            v += helper.step
            valueChanged( v.toFixed( helper.precision ), false )
          }

          // on press and hold behavior can be used from here:
          // https://github.com/mburakov/qt5/blob/master/qtquickcontrols/src/controls/SpinBox.qml#L306
        }

//        Rectangle {
//          color: "green"
//          opacity: .3
//          anchors.fill: parent
//        }
      }
    }
  }
}
