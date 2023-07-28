/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ANDROIDTRACKINGBROADCAST_H
#define ANDROIDTRACKINGBROADCAST_H

#include <QObject>
#include <QJniObject>

/**
 * \brief The AndroidTrackingBroadcast class is used as singleton to communicate
 * with Java Broadcast receiver via JNI and send messages from Java to Qt.
 */
class AndroidTrackingBroadcast : public QObject
{
    Q_OBJECT

  public:

    virtual ~AndroidTrackingBroadcast() = default;

    static AndroidTrackingBroadcast &getInstance()
    {
      static AndroidTrackingBroadcast instance;
      return instance;
    }

    static void notifyListenersPositionUpdated( JNIEnv *env, jobject /*this*/ )
    {
      AndroidTrackingBroadcast &trackingCallback = AndroidTrackingBroadcast::getInstance();

      emit trackingCallback.positionUpdated();
    }

    static void notifyListenersStatusUpdate( JNIEnv *env, jobject /*this*/, jstring message )
    {
      AndroidTrackingBroadcast &trackingCallback = AndroidTrackingBroadcast::getInstance();

      emit trackingCallback.statusChanged( env->GetStringUTFChars( message, 0 ) );
    }

    static void notifyListenersAliveResponse( JNIEnv *env, jobject /*this*/, jboolean isAlive )
    {
      AndroidTrackingBroadcast &trackingCallback = AndroidTrackingBroadcast::getInstance();

      emit trackingCallback.aliveResponse( isAlive );
    }

    static bool registerBroadcast()
    {
      return AndroidTrackingBroadcast::getInstance().registerBroadcastPrivate();
    }

    static bool unregisterBroadcast()
    {
      return AndroidTrackingBroadcast::getInstance().unregisterBroadcastPrivate();
    }

    static void sendAliveRequestAsync()
    {
      AndroidTrackingBroadcast::getInstance().sendAliveRequestAsyncPrivate();
    }

  signals:

    // Emitted when Java reports a new position
    void positionUpdated();

    // Emitted when the Java position service wants to report an issue or other status
    void statusChanged( const QString &message );

    // Emitted to notify if the tracking service is running
    void aliveResponse( bool isAlive );

  private:

    explicit AndroidTrackingBroadcast( QObject *parent = nullptr )
    {
      mBroadcastReceiver = QJniObject( "uk/co/lutraconsulting/PositionTrackingBroadcastMiddleware" );
    }

    bool registerBroadcastPrivate();
    bool unregisterBroadcastPrivate();
    void sendAliveRequestAsyncPrivate();

    bool mBroadcastIsRegistered = false;

    QJniObject mBroadcastReceiver;
};

#endif // ANDROIDTRACKINGBROADCAST_H
