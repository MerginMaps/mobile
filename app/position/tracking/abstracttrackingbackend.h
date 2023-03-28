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

class AbstractTrackingBackend : public QObject
{
    Q_OBJECT
  public:
    explicit AbstractTrackingBackend( QObject *parent = nullptr );

  signals:

};

#endif // ABSTRACTTRACKINGBACKEND_H
