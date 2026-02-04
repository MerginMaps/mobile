/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ANDROIDPOSITIONPROVIDER_H
#define ANDROIDPOSITIONPROVIDER_H

#include "abstractpositionprovider.h"

#include <QJniObject>

/**
 * AndroidPositionProvider uses Android's LocationManager API (when fused=false)
 * or Fused Location Provider from Google Play Services (when fused=true).
 *
 * Compared to Qt Positioning, it can use Fused Location Provider and it is
 * potentially more flexible because we are not going through a generic
 * positioning API.
 */
class AndroidPositionProvider : public AbstractPositionProvider
{
    Q_OBJECT

  public:
    explicit AndroidPositionProvider( bool fused, PositionTransformer &positionTransformer, QObject *parent = nullptr );
    ~AndroidPositionProvider() override;

    void startUpdates() override;
    void stopUpdates() override;
    void closeProvider() override;

    // Exposes PositionTransformer process function, as we need to access it outside the scope of this class
    GeoPosition processElevation( const GeoPosition &position );

    //! Checks whether the fused location provider can be used (i.e. Google Play services are present)
    static bool isFusedAvailable();
    //! If fused provider is not available, returns error string that could be presented to users.
    //! It is not very human friendly, but at least something (e.g. "SERVICE_DISABLED")
    static QString fusedErrorString();

  private:
    bool mFused;
    int mInstanceId;
    QJniObject mAndroidPos;

  public:
    // Multiple PositionProvider instances may exist at a time (because a new provider
    // gets created before the old one gets deleted), and our JNI callback method needs
    // to know to which instance to deliver a location update.
    static QMap<int, AndroidPositionProvider *> sInstances;
    static int sLastInstanceId;
};

#endif // ANDROIDPOSITIONPROVIDER_H
