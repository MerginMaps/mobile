/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef NETWORKPOSITIONPROVIDER_H
#define NETWORKPOSITIONPROVIDER_H

#include <QTcpSocket>
#include <QUdpSocket>

#include "abstractpositionprovider.h"
#include "bluetoothpositionprovider.h"


class NetworkPositionProvider : public AbstractPositionProvider
{
    Q_OBJECT

// signalizes in how many [ms] we will try to reconnect to GPS again
    enum ReconnectDelay
    {
      ShortDelay = 3000,
      LongDelay = 5000
    };

  public:
    explicit NetworkPositionProvider( const QString &addr, const QString &name, QObject *parent = nullptr );

    void startUpdates() override;
    void stopUpdates() override;
    void closeProvider() override;

  public slots:
    void positionUpdateReceived();
    void socketStateChanged( QAbstractSocket::SocketState state );
    void reconnectTimeout();

  private:
// utility function, which should return the socket in use
    QAbstractSocket *activeSocket() const;
// trigger the reconnection flow for both sockets
    void reconnect();
    // start the reconnection timeout
    void startReconnectTimer();

    std::unique_ptr<QTcpSocket> mTcpSocket;
    std::unique_ptr<QUdpSocket> mUdpSocket;

    int mReconnectDelay = ReconnectDelay::ShortDelay; // in how many [ms] we will try to reconnect again
    int mSecondsLeftToReconnect; // how many seconds are left to reconnect. Reconnects if less than or equal to one
    QTimer mReconnectTimer; // timer that times out each second and lowers the mSecondsLeftToReconnect by one

    QString mTargetAddress; // IP address or hostname of the receiver
    int mTargetPort; // active port of the receiver

    NmeaParser mNmeaParser; // parser to decode received NMEA strings
};
#endif //NETWORKPOSITIONPROVIDER_H
