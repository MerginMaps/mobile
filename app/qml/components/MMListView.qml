/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

//
// Hot-fix for hotfix https://github.com/MerginMaps/mobile/issues/3417
// Seems like there is some issue with cache in ListView
//

ListView {

  cacheBuffer: 0
}
