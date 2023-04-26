/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IOSTRACKINGBACKEND_H
#define IOSTRACKINGBACKEND_H

#include "abstracttrackingbackend.h"

#include <QObject>
#include <qglobal.h>

Q_FORWARD_DECLARE_OBJC_CLASS( IOSTrackingBackendImpl );

class IOSTrackingBackend : public AbstractTrackingBackend
{
    Q_OBJECT

  public:
    explicit IOSTrackingBackend( AbstractTrackingBackend::UpdateFrequency frequency, QObject *parent = nullptr );
    ~IOSTrackingBackend();

    // methods called from obj-c
    void positionUpdate( double longitude, double latitude, double altitude );
    void stopTrying( NSString *message );
    void logError( NSString *message );
    // ---

  private:
    void startPositionProvider( IOSTrackingBackend *observer );
    void releaseObjc();

    QString TAG = QStringLiteral( "IOS Tracking backend" );

    double mDistanceFilter = 0;
    IOSTrackingBackendImpl *mBackendImpl = nullptr; // owned, in objective-c
};

#endif // IOSTRACKINGBACKEND_H
