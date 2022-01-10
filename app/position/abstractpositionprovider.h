/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABSTRACTPOSITIONPROVIDER_H
#define ABSTRACTPOSITIONPROVIDER_H

#include "qgsgpsconnection.h"

#include "qobject.h"

class AbstractPositionProvider : public QObject
{
  Q_OBJECT

  public:
    AbstractPositionProvider( QObject *object = nullptr );
    virtual ~AbstractPositionProvider();

    virtual void close();
    virtual void stopUpdates();
    virtual void startUpdates();

  signals:
    void positionChanged( QgsGpsInformation position );
    void lostConnection();

  private:
    QgsGpsInformation mLastPosition;
};


#endif // ABSTRACTPOSITIONPROVIDER_H
