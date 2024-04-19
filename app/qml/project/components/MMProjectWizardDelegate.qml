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

import "../../components" as MMComponents
import "../../inputs" as MMInputs

Item {
  id: root

  property string attrname

  property alias comboboxField: combobox

  signal removeClicked()
  signal attrNameChanged( string attrname )

  implicitHeight: row.implicitHeight

  RowLayout {
    id: row

    height: parent.height
    width: parent.width

    spacing: __style.margin13

    MMInputs.MMTextInput {
      id: textField

      Layout.fillWidth: true

      text: root.attrname

      onTextChanged: () => root.attrNameChanged( text )
    }

    MMInputs.MMComboboxInput {
      id: combobox

      Layout.fillWidth: true
    }

    MMComponents.MMRoundButton {
      id: imageBtn

      iconSource: __style.deleteIcon
      iconColor: __style.grapeColor
      bgndColor: __style.negativeColor
      bgndHoverColor: __style.negativeColor

      onClicked: root.removeClicked()
    }
  }
}
