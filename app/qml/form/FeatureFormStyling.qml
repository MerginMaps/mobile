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
import ".."

QtObject {
  property color backgroundColor: "white"
  property real backgroundOpacity: 1
  property real titleLabelPointSize: 16

  property QtObject group: QtObject {
    property color backgroundColor: InputStyle.panelBackgroundLight
    property color marginColor: InputStyle.panelBackgroundDark
    property real leftMargin: 0 * QgsQuick.Utils.dp
    property real rightMargin: 0 * QgsQuick.Utils.dp
    property real topMargin: 1 * QgsQuick.Utils.dp
    property real bottomMargin: 1 * QgsQuick.Utils.dp
    property real height: 64 * QgsQuick.Utils.dp
    property color fontColor: InputStyle.fontColor
    property int spacing: InputStyle.formSpacing
    property int fontPixelSize: 24 * QgsQuick.Utils.dp
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
    property int tabLabelPointSize: 12
    property real borderWidth: 1 * QgsQuick.Utils.dp
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
    property real size: 80 * QgsQuick.Utils.dp
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
    property real cornerRadius: 8 * QgsQuick.Utils.dp
    property real height: InputStyle.fieldHeight
    property int fontPixelSize: 22 * QgsQuick.Utils.dp
    property real sideMargin: InputStyle.innerFieldMargin
    property real outerMargin: InputStyle.outerFieldMargin
    property int fontPointSize: 15
    property int labelPointSize: 12
  }

  property QtObject icons: QtObject {
    property var camera: InputStyle.cameraIcon
    property var remove: InputStyle.removeIcon
    property var gallery:InputStyle.galleryIcon
    property var brokenImage: __inputUtils.getThemeIcon("ic_broken_image_black")
    property var notAvailable: __inputUtils.getThemeIcon("ic_photo_notavailable_white")
    property var today: __inputUtils.getThemeIcon("ic_today")
    property var back: InputStyle.backIcon
    property var combobox: InputStyle.comboboxIcon
    property var valueRelationMore: InputStyle.valueRelationIcon
    property var importData: InputStyle.qrCodeIcon
    property var minus: __inputUtils.getThemeIcon("minus")
    property var plus: __inputUtils.getThemeIcon("plus-big")
  }

  property QtObject checkboxComponent: QtObject {
    property color baseColor: InputStyle.panelBackgroundDarker
  }

  property QtObject relationComponent: QtObject {
    property real textDelegateHeight: fields.height * 0.8
  }
}
