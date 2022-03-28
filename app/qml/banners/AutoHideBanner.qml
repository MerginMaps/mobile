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

Banner {
  id: root

  function show( interval = 3000 )
  {
    hideTimer.interval = interval
    hideTimer.start()
  }

  showBanner: hideTimer.running

  Timer {
    id: hideTimer

    interval: 3000
    running: false
    repeat: false
  }
}
