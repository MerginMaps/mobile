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
  * MMBaseSingleLineInput serves as a base class for all inputs that can benefit from
  * predefined textfield and/or left/right icons (actions).
  *
  * Private class, do not use standalone in the app.
  */

MMBaseInput {
  id: root

  property alias text: textFieldControl.text
  property alias placeholderText: textFieldControl.placeholderText

  property alias textField: textFieldControl
  property alias textFieldBackground: textFieldBackgroundGroup

  property alias leftContent: leftContentGroup.children
  property alias rightContent: rightContentGroup.children
  property alias leftContentMouseArea: leftContentMouseAreaGroup
  property alias rightContentMouseArea: rightContentMouseAreaGroup

  property bool rightContentVisible: rightContentGroup.children.length > 0
  property bool leftContentVisible: leftContentGroup.children.length > 0

  // Use instead of textField.inputMethodHints in inputs that can be read-only, overriding it drops the read-only hint
  property int textFieldInputMethodHints: Qt.ImhNone

  // Long press copies the whole value of fields the user can't edit
  property bool copyOnLongPress: root.editState !== "enabled"

  // IconColor is not used in this file directly, but derived components can use it
  // as precalculated color for icons to avoid repeating the same text over and over.
  property color iconColor: {
    if ( root.editState !== "enabled" ) return __style.mediumGreyColor
    if ( root.validationState === "error" ) return __style.grapeColor
    if ( root.validationState === "warning" ) return __style.earthColor
    return __style.forestColor
  }

  // Text edited signal is emitted when user edits the text manually,
  // you won't receive it when text is changed programatically.
  // Use textChanged if you need to listen to any text change.
  signal textEdited( string text )

  signal textClicked()
  signal leftContentClicked()
  signal rightContentClicked()

  inputContent: Rectangle {
    id: textFieldBackgroundGroup

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
      return 0
    }

    border.color: {
      if ( root.editState !== "enabled" ) return __style.polarColor
      if ( root.validationState === "error" ) return __style.negativeColor
      if ( root.validationState === "warning" ) return __style.warningColor
      if ( textFieldControl.activeFocus ) return __style.forestColor
      if ( textFieldControl.hovered ) return __style.forestColor

      return __style.polarColor
    }

    radius: __style.radius12

    RowLayout {
      anchors.fill: parent

      spacing: __style.margin12

      Item {
        id: leftContentGroupContainer

        Layout.preferredHeight: leftContentGroup.height
        Layout.preferredWidth: leftContentGroup.width
        Layout.leftMargin: __style.margin20

        visible: leftContentVisible

        Item {
          id: leftContentGroup

          width: childrenRect.width
          height: childrenRect.height

        }

        MouseArea {
          id: leftContentMouseAreaGroup

          anchors {
            fill: parent

            leftMargin: -__style.margin20
            topMargin: -__style.margin16
            rightMargin: -__style.margin12
            bottomMargin: -__style.margin16
          }

          onClicked: function( mouse ) {
            if ( root.editState === "enabled" ) {
                mouse.accepted = true
                root.focus = true
                root.leftContentClicked()
            }
          }
        }
      }

      TextField {
        id: textFieldControl

        Layout.fillWidth: true
        Layout.preferredHeight: parent.height

        // Do not let TextField calculate implicitWidth automatically based on background, it causes binding loops
        implicitWidth: width

        readOnly: root.editState === "readOnly" || root.editState === "disabled"
        activeFocusOnPress: !readOnly // Important due to crashes on iOS, see https://github.com/MerginMaps/mobile/pull/4561

        // Native selection in read-only fields is broken (QTBUG-141929), e.g. iOS double tap opens Select / Select All
        // with nothing selected, so turn it off completely and copy the whole value on long press instead
        inputMethodHints: root.textFieldInputMethodHints | ( readOnly ? Qt.ImhNoTextHandles : Qt.ImhNone )

        // Ensure the text is scrolled to the beginning
        Component.onCompleted: ensureVisible( 0 )

        color: {
          if ( root.editState === "readOnly" ) return __style.nightColor
          if ( root.editState === "enabled" ) return __style.nightColor
          if ( root.editState === "disabled" ) return __style.mediumGreyColor
          return __style.nightColor
        }

        topPadding: 0
        bottomPadding: 0
        leftPadding: leftContentGroupContainer.visible ? 0 : __style.margin20
        rightPadding: rightContentGroupContainer.visible ? 0 : __style.margin20

        placeholderTextColor: __style.darkGreyColor

        font: __style.p5

        background: Rectangle { color: __style.transparentColor }

        onTextEdited: root.textEdited( text )

        onPreeditTextChanged: if( __androidUtils.isAndroid ) Qt.inputMethod.commit() // to avoid Android's unconfirmed text

        // Android's native long press only selects a word in the focused field, but TextField holds the press back
        // and gives the focus only on release. Then the held-back press arrives, Qt takes it as a tap on the selection
        // and clears it. So focus the field before Android's long press comes and restore the selection on release.
        property bool androidLongPressed: false

        onPressed: androidLongPressed = false

        onReleased: {
          if ( root.editState !== "readOnly" ) {
            root.textClicked()
          }

          if ( androidLongPressed && selectedText !== "" ) {
            const start = selectionStart
            const end = selectionEnd
            Qt.callLater( () => select( start, end ) )
          }
          androidLongPressed = false
        }

        TapHandler {
          enabled: __androidUtils.isAndroid && !textFieldControl.readOnly
          longPressThreshold: 0.3 // Android's native long press comes after 0.4 s

          onLongPressed: {
            textFieldControl.forceActiveFocus()
            textFieldControl.androidLongPressed = true
          }
        }

        // TapHandler cancels the long press once the finger moves (e.g. scrolling the form), TextField's pressAndHold does not
        TapHandler {
          enabled: root.copyOnLongPress

          onLongPressed: {
            if ( textFieldControl.text.trim() === "" ) return

            textFieldControl.selectAll()
            textFieldControl.copy()
            textFieldControl.deselect()
            __notificationModel.addSuccess( qsTr( "Copied to clipboard" ) )
          }
        }
      }

      Item {
        id: rightContentGroupContainer

        Layout.preferredHeight: rightContentGroup.height
        Layout.preferredWidth: rightContentGroup.width
        Layout.rightMargin: __style.margin20

        visible: rightContentVisible

        Item {
          id: rightContentGroup

          width: childrenRect.width
          height: childrenRect.height
        }

        MouseArea {
          id: rightContentMouseAreaGroup

          anchors {
            fill: parent

            leftMargin: -__style.margin12
            topMargin: -__style.margin16
            rightMargin: -__style.margin20
            bottomMargin: -__style.margin16
          }

          onClicked: function( mouse ) {
            if ( root.editState === "enabled" ) {
              mouse.accepted = true
              root.focus = true
              root.rightContentClicked()
            }
          }
        }
      }
    }
  }
}
