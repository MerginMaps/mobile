/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TRIMBLEPOSITIONPROVIDER_H
#define TRIMBLEPOSITIONPROVIDER_H

#include <QTimer>
#include <QWebSocket>

#include "abstractpositionprovider.h"
#include "trimbleregistration.h"

class TrimblePositionProvider : public AbstractPositionProvider
{
    Q_OBJECT

    enum ReconnectDelay
    {
      ShortDelay = 3000,
      LongDelay = 5000,
      ExtraLongDelay = 10000
    };

  public:
    TrimblePositionProvider( const QString &id, const QString &name, PositionTransformer &positionTransformer, QObject *parent = nullptr );
    ~TrimblePositionProvider() override;

    void startUpdates() override;
    void stopUpdates() override;
    void closeProvider() override;

    QgsCoordinateReferenceSystem sourceCrs() const override;

    Q_INVOKABLE void openAntennaHeightPage();

    // Visible for unit testing
    static GeoPosition parseLocationMessage( const QString &json );
    static QgsCoordinateReferenceSystem resolveFrame( const QString &frameName, double epoch );

  private slots:
    void onRegistered( int port );
    void onRegistrationFailed( const QString &reason );
    void onTextMessageReceived( const QString &message );
    void onSocketError( QAbstractSocket::SocketError error );
    void onSocketDisconnected();
    void onReconnectTimeout();

  private:
    void connectWebSocket( int port );
    void startReconnectTimer();
    void reconnect();

    TrimbleRegistration *mRegistration = nullptr;
    std::unique_ptr<QWebSocket> mSocket;
    QTimer mReconnectTimer;
    QTimer mHeartBeatTimer;

    int mCachedPort = 0;
    int mReconnectDelay = ReconnectDelay::ShortDelay;
    int mSecondsLeftToReconnect = 0;

    QgsCoordinateReferenceSystem mSourceCrs;
};

#endif // TRIMBLEPOSITIONPROVIDER_H
