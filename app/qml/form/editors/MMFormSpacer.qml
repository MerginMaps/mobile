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

import "../../components"
import "../../inputs"

/*
 * Spacer for QGIS Attribute Form
 * Has 2 forms: with HLine and without
 * It does not have title
 * Read-Only, user cannot modify the content
 *
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 */

Rectangle {
  id: root

  property var _fieldConfig: parent.fieldConfig

  height: 1 * __dp < 1 ? 1 : 1 * __dp // parent form's list inserts space between each 2 elements
  width: parent.width

  color: _fieldConfig["IsHLine"] ? __style.forestColor : "transparent"

  anchors {
    right: parent.right
    left: parent.left
  }

}
