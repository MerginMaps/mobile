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

            if ( !Number.isNaN(v) )
            {
              v -= helper.step
              valueChanged( v.toFixed( helper.precision ), false )
            }
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

            // The following line is unfortunatelly causing Layout polish loop, needs to be fixed
            Layout.preferredWidth: parent.width / 2 + numberInput.contentWidth * 0.38

            TextInput {
              id: numberInput

              onEditingFinished: {
                let v = toNumber( numberInput.displayText )

                if ( Number.isNaN( v ) )
                {
                  valueChanged( numberInput.displayText, numberInput.displayText === "" )
                }
                else
                {
                  valueChanged( v, numberInput.displayText === "" )
                }
              }

              anchors.fill: parent

              text: fieldItem.parent.value !== undefined ? Number( fieldItem.parent.value ).toLocaleString( locale, 'f', helper.precision ) : ""

              inputMethodHints: helper.precision === 0 ? Qt.ImhDigitsOnly : Qt.ImhFormattedNumbersOnly
              font.pointSize: customStyle.fields.fontPointSize
              color: customStyle.fields.fontColor
              selectionColor: customStyle.fields.fontColor
              selectedTextColor: "#ffffff"

              horizontalAlignment: Qt.AlignRight
              verticalAlignment: Qt.AlignVCenter

              clip: true
            }
          }

          Item {
            id: suffixContainer

            Layout.preferredHeight: parent.height
            Layout.maximumHeight: parent.height
            Layout.fillWidth: true
            Layout.minimumWidth: suffix.paintedWidth

            Text {
              id: suffix

              text: helper.suffix

              visible: helper.suffix !== "" && numberInput.text !== ""

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

            if ( !Number.isNaN( v ) )
            {
              v += helper.step
              valueChanged( v.toFixed( helper.precision ), false )
            }
          }

          // on press and hold behavior can be used from here:
          // https://github.com/mburakov/qt5/blob/93bfa3874c10f6cb5aa376f24363513ba8264117/qtquickcontrols/src/controls/SpinBox.qml#L306-L309
        }
      }
    }
  }
}
