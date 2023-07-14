/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ANDROIDTRACKINGCALLBACK_H
#define ANDROIDTRACKINGCALLBACK_H

#include <QObject>
#include <QJniObject>

#include "position/tracking/androidtrackingbackend.h"

/**
 * \brief The AndroidTrackingCallback class is used as singleton to communicate
 * with Broadcast receiver via JNI and send messages from Java further to Qt.
 */
class AndroidTrackingCallback : public QObject
{
    Q_OBJECT

  public:

    virtual ~AndroidTrackingCallback() = default;

    static AndroidTrackingCallback &getInstance()
    {
      static AndroidTrackingCallback instance;
      return instance;
    }

    static void notifyListenersPositionUpdated( JNIEnv *env, jobject /*this*/ )
    {
      AndroidTrackingCallback &trackingCallback = AndroidTrackingCallback::getInstance();

      if ( trackingCallback.mBackend )
      {
        trackingCallback.mBackend->sourceUpdatedPosition();
      }
    }

    static void notifyListenersStatusUpdate( JNIEnv *env, jobject /*this*/, jstring message )
    {
      AndroidTrackingCallback &trackingCallback = AndroidTrackingCallback::getInstance();

      if ( trackingCallback.mBackend )
      {
        trackingCallback.mBackend->sourceUpdatedState( env->GetStringUTFChars( message, 0 ) );
      }
    }

    void setBackend( AndroidTrackingBackend *backend )
    {
      if ( backend )
      {
        qDebug() << "Set callback backend" << backend;
      }
      else
      {
        qDebug() << "Set callback backend nullptr";
      }

      mBackend = backend;
    }

  private:
    explicit AndroidTrackingCallback( QObject *parent = nullptr ) { }

    AndroidTrackingBackend *mBackend = nullptr;
};

#endif // ANDROIDTRACKINGCALLBACK_H
