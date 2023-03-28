/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SIMULATEDTRACKINGBACKEND_H
#define SIMULATEDTRACKINGBACKEND_H

#include "abstracttrackingbackend.h"
#include <qglobal.h>

class AbstractPositionProvider;

class SimulatedTrackingBackend : public AbstractTrackingBackend
{
    Q_OBJECT
  public:
    explicit SimulatedTrackingBackend( AbstractPositionProvider *positionProvider, AbstractTrackingBackend::UpdateFrequency updateFrequency, QObject *parent = nullptr );
    explicit SimulatedTrackingBackend( AbstractTrackingBackend::UpdateFrequency updateFrequency, QObject *parent = nullptr );

  private:

    std::unique_ptr<AbstractPositionProvider> mPositionProvider;
};

#endif // SIMULATEDTRACKINGBACKEND_H
