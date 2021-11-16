/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.14
import QgsQuick 0.1 as QgsQuick
import "../"

QtObject {
  property color backgroundColor: "white"
  property real backgroundOpacity: 1
  property QtObject checkboxComponent: QtObject {
    property color baseColor: InputStyle.panelBackgroundDarker
  }
  property QtObject constraint: QtObject {
    property color descriptionColor: "#e67e22"
    property color invalidColor: "#c0392b"
    property color validColor: InputStyle.labelColor
  }
  property QtObject fields: QtObject {
    property color activeColor: InputStyle.fontColor
    property color attentionColor: "#aa0000"
    property color backgroundColor: InputStyle.panelBackgroundLight
    property color backgroundColorDark: InputStyle.panelBackgroundDark
    property color backgroundColorDarker: InputStyle.panelBackgroundDarker
    property color backgroundColorInactive: "grey"
    property real cornerRadius: 8 * QgsQuick.Utils.dp
    property color fontColor: InputStyle.fontColor
    property int fontPixelSize: 22 * QgsQuick.Utils.dp
    property int fontPointSize: 15
    property real height: InputStyle.fieldHeight
    property int labelPointSize: 12
    property color normalColor: InputStyle.panelBackgroundLight
    property real outerMargin: InputStyle.outerFieldMargin
    property real sideMargin: InputStyle.innerFieldMargin
  }
  property QtObject group: QtObject {
    property color backgroundColor: InputStyle.panelBackgroundLight
    property real bottomMargin: 1 * QgsQuick.Utils.dp
    property color fontColor: InputStyle.fontColor
    property int fontPixelSize: 24 * QgsQuick.Utils.dp
    property real height: 64 * QgsQuick.Utils.dp
    property real leftMargin: 0 * QgsQuick.Utils.dp
    property color marginColor: InputStyle.panelBackgroundDark
    property real rightMargin: 0 * QgsQuick.Utils.dp
    property int spacing: InputStyle.formSpacing
    property real topMargin: 1 * QgsQuick.Utils.dp
  }
  property QtObject icons: QtObject {
    property var back: InputStyle.backIcon
    property var camera: InputStyle.cameraIcon
    property var combobox: InputStyle.comboboxIcon
    property var gallery: InputStyle.galleryIcon
    property var importData: InputStyle.qrCodeIcon
    property var minus: __inputUtils.getThemeIcon("minus")
    property var notAvailable: __inputUtils.getThemeIcon("no-image")
    property var plus: __inputUtils.getThemeIcon("plus-big")
    property string relationsLink: InputStyle.linkIcon
    property string relationsUnlink: InputStyle.unlinkIcon
    property var remove: InputStyle.removeIcon
    property var today: __inputUtils.getThemeIcon("ic_today")
    property var valueRelationMore: InputStyle.valueRelationIcon
  }
  property QtObject relationComponent: QtObject {
    property int flowSpacing: 8 * QgsQuick.Utils.dp
    property color iconColor: InputStyle.darkGreen
    property color iconColorButton: InputStyle.darkOrange

    // photo mode
    property color photoBorderColor: InputStyle.darkGreen
    property color photoBorderColorButton: InputStyle.darkOrange
    property real photoBorderWidth: 2 * QgsQuick.Utils.dp

    // tag cloud (text mode)
    property color tagBackgroundColor: InputStyle.darkGreen
    property color tagBackgroundColorButton: InputStyle.darkOrange
    property color tagBackgroundColorButtonAlt: InputStyle.panelBackgroundLight
    property color tagBorderColor: InputStyle.darkGreen
    property color tagBorderColorButton: InputStyle.darkOrange
    property real tagBorderWidth: 2 * QgsQuick.Utils.dp
    property real tagInnerSpacing: 30 * QgsQuick.Utils.dp
    property real tagRadius: 10 * QgsQuick.Utils.dp
    property color tagTextColor: InputStyle.panelBackgroundLight
    property color tagTextColorButton: InputStyle.darkOrange
    property color textColorButton: InputStyle.darkOrange
    property real textDelegateHeight: fields.height * 0.8
  }
  property QtObject tabs: QtObject {
    property color activeBackgroundColor: InputStyle.panelBackgroundLight
    property color activeColor: InputStyle.fontColor
    property color backgroundColor: InputStyle.panelBackgroundLight
    property color borderColor: InputStyle.labelColor
    property real borderWidth: 1 * QgsQuick.Utils.dp
    property real buttonHeight: height
    property color disabledBackgroundColor: InputStyle.panelBackgroundDark
    property color disabledColor: InputStyle.fontColor
    property real height: InputStyle.rowHeight * 0.9
    property color normalBackgroundColor: InputStyle.panelBackgroundLight
    property color normalColor: InputStyle.fontColor
    property real spacing: 0
    property int tabLabelPointSize: 12
  }
  property real titleLabelPointSize: 16
  property QtObject toolbutton: QtObject {
    property color activeButtonColor: InputStyle.activeButtonColor
    property color backgroundColor: "transparent"
    property color backgroundColorInvalid: "#bdc3c7"
    property real size: 80 * QgsQuick.Utils.dp
  }
}
