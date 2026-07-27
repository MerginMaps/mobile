/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TRIMBLEREGISTRATION_H
#define TRIMBLEREGISTRATION_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <memory>

#ifdef ANDROID
class TmmResultReceiver; // defined in trimbleregistration_android.cpp
#endif

/**
 * Platform-agnostic async contract for registering with Trimble Mobile Manager.
 *
 * Call requestRegistration() once; listen for registered() or failed().
 * Concrete implementations are in trimbleregistration_android.cpp (Android)
 * and trimbleregistration_ios.mm (iOS).
 */
class TrimbleRegistration : public QObject
{
    Q_OBJECT

  public:
    explicit TrimbleRegistration( QObject *parent = nullptr );
    ~TrimbleRegistration() override;

    void requestRegistration( const QString &appId );

    // iOS only: called by QDesktopServices URL handler when TMM calls back
    Q_INVOKABLE void handleCallback( const QUrl &url );

  signals:
    void registered( int locationV2Port );
    void failed( const QString &reason );

  private:
#ifdef ANDROID
    std::unique_ptr<TmmResultReceiver> mResultReceiver;
#endif
};

#endif // TRIMBLEREGISTRATION_H
