/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABSTRACTTRACKINGBACKEND_H
#define ABSTRACTTRACKINGBACKEND_H

#include <QObject>
#include <qglobal.h>

#include "position/geoposition.h"

class AbstractTrackingBackend : public QObject
{
    Q_OBJECT

    Q_PROPERTY( UpdateFrequency updateFrequency READ updateFrequency WRITE setUpdateFrequency NOTIFY updateFrequencyChanged )

  public:
    enum UpdateFrequency
    {
      Often = 0,
      Normal,
      Occasional,
    };
    Q_ENUM( UpdateFrequency );

    explicit AbstractTrackingBackend( QObject *parent = nullptr );

    UpdateFrequency updateFrequency() const;
    void setUpdateFrequency( const UpdateFrequency &newUpdateFrequency );

  signals:
    void positionChanged( GeoPosition position );

    void updateFrequencyChanged( AbstractTrackingBackend::UpdateFrequency updateFrequency );

  private:
    UpdateFrequency mUpdateFrequency;
};

#endif // ABSTRACTTRACKINGBACKEND_H
