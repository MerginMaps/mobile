/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/**
 * This file should be used for explicit declaration of 3rd party types into QML. As well it's used to declare types
 * from core into QML.
 */

#ifndef MMTYPEUTILS_H
#define MMTYPEUTILS_H

#include <qgsgeometry.h>
#include <qqmlintegration.h>

#include "synchronizationerror.h"

struct ForeignGeometry
{
  Q_GADGET
  QML_FOREIGN( QgsGeometry )
  QML_VALUE_TYPE( qgsGeometry );
};

struct ForeignPoint
{
  Q_GADGET
  QML_FOREIGN( QgsPoint )
  QML_VALUE_TYPE( qgsPoint );
};

class SynchronizationErrorDerived : public SynchronizationError
{
    Q_GADGET
};

namespace SynchronizationErrorsDerivedForeign
{
  Q_NAMESPACE
  QML_NAMED_ELEMENT( SyncError )
  QML_FOREIGN_NAMESPACE( SynchronizationErrorDerived )
}


#endif //MMTYPEUTILS_H
