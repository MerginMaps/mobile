/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14

import ".."

QtObject {
  property color backgroundColor: "white"
  property real backgroundOpacity: 1
  property real titleLabelPixelSize: InputStyle.fontPixelSizeNormal

  property QtObject group: QtObject {
    property color backgroundColor: InputStyle.panelBackgroundLight
    property color marginColor: InputStyle.panelBackgroundDark
    property real leftMargin: 0 * __dp
    property real rightMargin: 0 * __dp
    property real topMargin: 1 * __dp
    property real bottomMargin: 1 * __dp
    property real height: 64 * __dp
    property color fontColor: InputStyle.fontColor
    property int spacing: InputStyle.formSpacing
    property int fontPixelSize: InputStyle.fontPixelSizeNormal
  }

  property QtObject tabs: QtObject {
    property color normalColor: InputStyle.fontColor
    property color activeColor: InputStyle.fontColor
    property color disabledColor: InputStyle.fontColor
    property color backgroundColor: InputStyle.panelBackgroundLight
    property color normalBackgroundColor: InputStyle.panelBackgroundLight
    property color activeBackgroundColor: InputStyle.panelBackgroundLight
    property color disabledBackgroundColor: InputStyle.panelBackgroundDark
    property real height: InputStyle.rowHeight * 0.9
    property real buttonHeight: height
    property real spacing: 0
    property int tabLabelPixelSize: InputStyle.fontPixelSizeSmall
    property real borderWidth: 1 * __dp
    property color borderColor: InputStyle.labelColor
  }

  property QtObject constraint: QtObject {
    property color validColor: InputStyle.labelColor
    property color invalidColor: "#c0392b"
    property color descriptionColor: "#e67e22"
  }

  property QtObject toolbutton: QtObject {
    property color backgroundColor: "transparent"
    property color backgroundColorInvalid: "#bdc3c7"
    property color activeButtonColor: InputStyle.activeButtonColor
    property real size: 80 * __dp
  }

  property QtObject fields: QtObject {
    property color backgroundColor: InputStyle.panelBackgroundLight
    property color backgroundColorDark: InputStyle.panelBackgroundDark
    property color backgroundColorDarker: InputStyle.panelBackgroundDarker
    property color backgroundColorInactive: "grey"
    property color fontColor: InputStyle.fontColor
    property color activeColor: InputStyle.fontColor
    property color attentionColor: "#aa0000"
    property color normalColor: InputStyle.panelBackgroundLight
    property real cornerRadius: 8 * __dp
    property real height: InputStyle.fieldHeight
    property int fontPixelStyle: InputStyle.fontPixelSizeSmall
    property real sideMargin: InputStyle.innerFieldMargin
    property real outerMargin: InputStyle.outerFieldMargin
    property int fontPixelSize: InputStyle.fontPixelSizeNormal
    property int labelPixelSize: InputStyle.fontPixelSizeSmall
  }

  property QtObject icons: QtObject {
    property var camera: InputStyle.cameraIcon
    property var remove: InputStyle.removeIcon
    property var gallery:InputStyle.galleryIcon
    property var notAvailable: __inputUtils.getThemeIcon("no-image")
    property var today: __inputUtils.getThemeIcon("ic_today")
    property var back: InputStyle.backIcon
    property var combobox: InputStyle.comboboxIcon
    property var valueRelationMore: InputStyle.valueRelationIcon
    property var importData: InputStyle.qrCodeIcon
    property var minus: __inputUtils.getThemeIcon("minus")
    property var plus: __inputUtils.getThemeIcon("plus-big")
    property string relationsLink: InputStyle.linkIcon
    property string relationsUnlink: InputStyle.unlinkIcon
  }

  property QtObject checkboxComponent: QtObject {
    property color baseColor: InputStyle.panelBackgroundDarker
  }

  property QtObject relationComponent: QtObject {
    property real textDelegateHeight: fields.height * 0.8
    property int flowSpacing: 8 * __dp

    // photo mode
    property color photoBorderColor: InputStyle.darkGreen
    property color photoBorderColorButton: InputStyle.darkOrange
    property color iconColor: InputStyle.darkGreen
    property color iconColorButton: InputStyle.darkOrange
    property color textColorButton: InputStyle.darkOrange
    property real photoBorderWidth: 2 * __dp

    // tag cloud (text mode)
    property color tagBackgroundColor: InputStyle.darkGreen
    property color tagBackgroundColorButton: InputStyle.darkOrange
    property color tagBackgroundColorButtonAlt: InputStyle.panelBackgroundLight
    property color tagBorderColor: InputStyle.darkGreen
    property color tagBorderColorButton: InputStyle.darkOrange
    property color tagTextColor: InputStyle.panelBackgroundLight
    property color tagTextColorButton: InputStyle.darkOrange
    property real tagBorderWidth: 2 * __dp
    property real tagRadius: 10 * __dp
    property real tagInnerSpacing: 30 * __dp
  }
}
