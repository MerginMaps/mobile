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

#include "qgspoint.h"

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

    enum SignalSlotSupport
    {
      Supported = 0,
      NotSupported
    };
    Q_ENUM( SignalSlotSupport );

    explicit AbstractTrackingBackend( UpdateFrequency updateFrequency = Often, SignalSlotSupport signalSlotSupport = Supported, QObject *parent = nullptr );

    UpdateFrequency updateFrequency() const;
    void setUpdateFrequency( const UpdateFrequency &newUpdateFrequency );

    SignalSlotSupport signalSlotSupport() const;

    void setNotifyFunction( std::function<void( const QgsPoint &position )> );

    virtual void appMinimized() {};
    virtual void appMaximized() {};

  signals:
    void positionChanged( const QgsPoint &position );
    void multiplePositionChanges( QList<QgsPoint> positions );

    void errorOccured( const QString &error );
    void abort();

    void updateFrequencyChanged( AbstractTrackingBackend::UpdateFrequency updateFrequency );

  protected:
    void notifyListeners( const QgsPoint &position );
    void setSignalSlotSupport( SignalSlotSupport support );

  private:
    UpdateFrequency mUpdateFrequency;
    SignalSlotSupport mSignalSlotSupport;

    //! Function to call when this provider does not support signal/slot connection
    std::function<void( const QgsPoint &position )> mNotifyFunction;
};

#endif // ABSTRACTTRACKINGBACKEND_H
