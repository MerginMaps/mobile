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

// To ignore the warning "The current style does not support customization"
// see from https://stackoverflow.com/questions/76625756/the-current-style-does-not-support-customization-of-this-control
import QtQuick.Controls.Basic

/**
  * MMBaseTextInput serves as a base class for all inputs that can benefit from
  * predefined textfield and/or left/right icons (actions).
  *
  * Private class, do not use standalone in the app.
  */

MMBaseInput {
  id: root

  property alias text: textFieldControl.text
  property alias textField: textFieldControl
  property alias leftContent: leftContentGroup.children
  property alias rightContent: rightContentGroup.children

  inputContent: Rectangle {
    width: parent.width
    height: __style.row50

    color: {
      if ( root.editState !== "enabled" ) return __style.polarColor
      if ( root.validationState === "error" ) return __style.negativeUltraLightColor
      if ( root.validationState === "warning" ) return __style.negativeUltraLightColor

      return __style.polarColor
    }

    border.width: {
      if ( root.validationState === "error" ) return __style.width2
      if ( root.validationState === "warning" ) return __style.width2
      if ( textFieldControl.activeFocus ) return __style.width2
      if ( textFieldControl.hovered ) return __style.width1
//      if ( contentBgndMouseArea.containsMouse ) return __style.width1
      return 0
    }

    border.color: {
      if ( root.editState !== "enabled" ) return __style.polarColor
      if ( root.validationState === "error" ) return __style.negativeColor
      if ( root.validationState === "warning" ) return __style.warningColor
      if ( textFieldControl.activeFocus ) return __style.forestColor
      if ( textFieldControl.hovered ) return __style.forestColor
//      if ( contentBgndMouseArea.containsMouse ) return __style.width1

      return __style.polarColor
    }

    radius: __style.radius12

//    MouseArea { //-----> convert to left and Right MouseArea
//      id: contentBgndMouseArea

//      anchors.fill: parent
//      hoverEnabled: true

//      onClicked: function( mouse ) {
//        mouse.accepted = true
//        textFieldControl.forceActiveFocus()
//        textFieldControl.ensureVisible( textFieldControl.text.length )
//      }
//    }

    RowLayout {
      anchors {
        fill: parent

//        leftMargin: __style.margin20
//        rightMargin: __style.margin20

//        topMargin: __style.margin12
//        bottomMargin: __style.margin12
      }

      spacing: __style.margin12

      Item {
        id: leftContentGroup

        Layout.preferredHeight: childrenRect.height
        Layout.preferredWidth: childrenRect.width

        visible: children.length > 0
      }

      TextField {
        id: textFieldControl

        Layout.fillWidth: true
        Layout.preferredHeight: parent.height

        readOnly: root.editState === "readOnly" || root.editState === "disabled"

        horizontalAlignment: TextInput.AlignLeft

        // Ensure the text is scrolled to the begging
        Component.onCompleted: ensureVisible( 0 )

        color: {
          if ( root.editState === "readOnly" ) return __style.nightColor
          if ( root.editState === "enabled" ) return __style.nightColor
          if ( root.editState === "disabled" ) return __style.mediumGreyColor
        }

        leftPadding: 0
        topPadding: 0
        rightPadding: 0
        bottomPadding: 0

        placeholderTextColor: __style.darkGreyColor

        font: __style.p5

        background: Rectangle { color: __style.negativeColor }
      }

      Item {
        id: rightContentGroup

        Layout.preferredHeight: childrenRect.height
        Layout.preferredWidth: childrenRect.width

        visible: children.length > 0
      }
    }
  }
}
