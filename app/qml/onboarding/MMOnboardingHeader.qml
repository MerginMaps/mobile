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
import QtQuick.Controls

Item {
  id: root

  signal backClicked()

  /* translate in parent! */
  required property var headerTitle

  /* 0 no step bar shown; 1, 2, 3 */
  property int step: 0

  ColumnLayout {
      RowLayout {
          Label {
             text: headerTitle
          }

          ToolButton {
            onClicked: root.backClicked()
            text: "<"
          }

          ProgressBar {
              visible: step > 0
              from: 0
              to: 2
              value: step - 1
          }
      }
  }
}
