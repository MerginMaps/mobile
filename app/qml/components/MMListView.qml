/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

ListView {

  id: root
  // when flicking up really fast, we should go back to the first item
  onVerticalOvershootChanged: {
    if (verticalOvershoot < -200) {
      root.contentY= -root.topMargin
      root.returnToBounds();
    }
  }
}