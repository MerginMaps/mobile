/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts

import "../" as MMComponents

/**
  * MMBaseInput is a base class for all inputs in the app.
  * Inputs and form editors are derived from this component.
  *
  * Private class, do not use in the app.
  */

Item {
  id: root

  property alias title: titleText.text

  property bool hasCheckbox: false
  property alias checkboxChecked: checkbox.checked

  property bool readOnly: false
  property bool shouldShowValidation: true

  property string errorMsg: ""
  property string warningMsg: ""

  property alias inputContent: contentGroup.children

  property alias editState: editStateGroup.state
  property alias validationState: validationStateGroup.state

  StateGroup {
    id: editStateGroup

    states: [
      State {
        name: "enabled"
        when: enabled && !readOnly
      },
      State {
        name: "disabled"
        when: !enabled
      },
      State {
        name: "readOnly"
        when: enabled && readOnly
      }
    ]

    state: "enabled"
  }

  StateGroup {
    id: validationStateGroup

    states: [
      State {
        name: "valid"
        when: !warningMsg && !errorMsg
      },
      State {
        name: "error"
        when: errorMsg
      },
      State {
        name: "warning"
        when: warningMsg && !errorMsg
      }
    ]

    state: "valid"
  }

  implicitHeight: rootGroup.implicitHeight

  Column {
    id: rootGroup

    width: parent.width

    spacing: __style.margin4

    Item {
      // checkbox and title div

      width: parent.width
      implicitHeight: titleRowGroup.implicitHeight

      visible: root.title

      RowLayout {
        id: titleRowGroup

        width: parent.width

        // Checkbox has some padding to the right so we do not need spacing.
        // Once we refactor the checkbox, we need to add spacing here.
        spacing: 0

        MMComponents.MMCheckBox {
          id: checkbox

          small: true
          visible: root.hasCheckbox && root.editState === "enabled"

          emptyStateColor: __style.polarColor
          checked: root.checkboxChecked

          MouseArea {
            anchors {
              fill: parent
              margins: -__style.margin16
            }

            onClicked: function( mouse ) {
              mouse.accepted = true
              checkbox.toggle()
            }
          }

        }

        MMComponents.MMText {
          id: titleText

          text: root.title
          font: __style.p6

          Layout.fillWidth: true

          wrapMode: Text.Wrap
          maximumLineCount: 10

          MouseArea {
            width: parent.contentWidth + 2 * __style.margin12
            height: parent.contentHeight

            x: -__style.margin12

            onClicked: function( mouse ) {
              mouse.accepted = true

              if ( checkbox.visible ) {
                checkbox.toggle()
              }
            }
          }
        }
      }
    }

    Item {
      id: contentGroup

      width: parent.width
      height: childrenRect.height
    }

    Item {
      // validation messages

      width: parent.width
      height: validationMessagegroup.implicitHeight

      visible: root.validationState !== "valid" && root.shouldShowValidation

      RowLayout {
        id: validationMessagegroup

        width: parent.width

        MMComponents.MMIcon {
          source: __style.errorCircleIcon
          size: __style.icon16
          color: {
            if ( root.validationState === "error" ) return __style.negativeColor
            if ( root.validationState === "warning" ) return __style.warningColor
            return __style.forestColor
          }
        }

        MMComponents.MMText {
          Layout.fillWidth: true

          text: {
            if ( root.validationState === "error" ) return root.errorMsg
            if ( root.validationState === "warning" ) return root.warningMsg
            return ""
          }
          color: {
            if ( root.validationState === "error" ) return __style.grapeColor
            if ( root.validationState === "warning" ) return __style.earthColor
            return __style.forestColor
          }

          font: __style.t4


          wrapMode: Text.Wrap
          maximumLineCount: 10
        }
      }
    }
  }
}
