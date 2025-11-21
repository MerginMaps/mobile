/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABSTRACTMAPTOOL_H
#define ABSTRACTMAPTOOL_H

#include <QObject>
#include <qglobal.h>

#include "inputmapsettings.h"

class AbstractMapTool : public QObject
{
    Q_OBJECT

    Q_PROPERTY( InputMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )

  public:
    explicit AbstractMapTool( QObject *parent = nullptr );
    virtual ~AbstractMapTool();

    InputMapSettings *mapSettings() const;
    void setMapSettings( InputMapSettings *newMapSettings );

  signals:
    void onAboutToChangeMapSettings();
    void mapSettingsChanged( InputMapSettings *mapSettings );

  private:
    InputMapSettings *mMapSettings = nullptr;
};

#endif // ABSTRACTMAPTOOL_H
