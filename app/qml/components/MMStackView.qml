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

// StackView wrapper, which improves Squish support. Should be used instead of pure QML StackView.

StackView {
  id: root

  onEmptyChanged: function() {
    visible = !empty
  }

  Component.onCompleted: {
    if (empty) {
      visible = false
    }
  }
}
