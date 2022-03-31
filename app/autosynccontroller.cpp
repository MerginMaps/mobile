/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "autosynccontroller.h"
#include "coreutils.h"

#include "qgsproject.h"
#include "qgsvectorlayer.h"

AutosyncController::AutosyncController(
  QgsProject *openedQgsProject,
  QObject *parent
)
  : QObject( parent )
  , mQgsProject( openedQgsProject )
{
  if ( !mQgsProject )
  {
    CoreUtils::log( QStringLiteral( "Autosync" ), QStringLiteral( "Received an invalid active project data" ) );
    return;
  }

  // Register for data change of project's vector layers
  const QMap<QString, QgsMapLayer *> layers = mQgsProject->mapLayers( true );
  for ( const QgsMapLayer *layer : layers )
  {
    const QgsVectorLayer *vecLayer = qobject_cast<const QgsVectorLayer *>( layer );
    if ( vecLayer )
    {
      if ( !vecLayer->readOnly() )
      {
        QObject::connect( vecLayer, &QgsVectorLayer::afterCommitChanges, this, &AutosyncController::projectChangeDetected );
      }
    }
  }
}

AutosyncController::~AutosyncController() = default;
