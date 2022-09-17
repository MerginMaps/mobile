/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import ".."

Banner {
  id: root

  property int visibleInterval: 3000 // [ms]

  function show()
  {
    hideTimer.interval = root.visibleInterval
    hideTimer.start()
  }

  function hide()
  {
    hideTimer.stop()
  }

  showBanner: hideTimer.running

  Timer {
    id: hideTimer

    interval: 3000
    running: false
    repeat: false
  }
}
